//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <RLScheduler.h>
#include <omnetpp.h>

namespace ncqrs {

RLScheduler::RLScheduler(int nodes, int tdmaSF, int maxChunks, int retxChunks, int retxSlots, int offsID)
{
    this->numSSs = nodes;
    this->tdmaSF = tdmaSF;
    this->maxChunks = maxChunks;
    this->retxChunks = retxChunks;
    this->retxSlots = retxSlots;        // Max number allowed to be used by retransmissions (aka T)
    this->offsetID = offsID;
}
RLScheduler::~RLScheduler(){}

void RLScheduler::initialize(double pErrInitial, double rlAlfa, double rlBeta,
                             double rlGamma, double rlQAlfa, double rlQGamma,
                             int slotsPerUsr, double trimRank, double rewSlots)
{
    int i;
    /* Initialize RL (optional) ARG */
    pErr = new double[numSSs];

    aPkt = new pktConstruct(offsetID, maxChunks);

    badPkts = new long[numSSs];

    sPrio = new streamPrio_t[numSSs];

    td_error = new double[numSSs];

    corrupt_stream = new int[numSSs];

    slotsPerUser = round(2*log(retxSlots)); //slotsPerUsr

    EV << "User limited by " << slotsPerUser << " [slots]\n";

    // pass exploration curve settings
    tau = new double[numSSs];

    rewardSlots = rewSlots;

    trim_rank = trimRank;

    for (i = 0; i < numSSs; i++)
    {
        tau[i] = 1.0;
        pErr[i] = pErrInitial;   /* set all errors to some dummy value */
        badPkts[i] = 0;
        // sPrio is used for distance based priority scheduling
        sPrio[i].prio = 0;
        sPrio[i].id = i;
        td_error[i] = 0.0;
    }

    // Rewards per stream
    this->reward = new double[numSSs];
    ac.askRetx = new int[numSSs];
    ac.getRetx = new int[numSSs];
    ac.lbCoeff = new double[numSSs];
    ac.gibbs_list = new double*[numSSs];
    // A-C coefficients
    this->alfa = rlAlfa;
    this->beta = rlBeta;
    this->gamma = rlGamma;
    // Q-Learning coefficients
    this->qalfa = rlQAlfa;
    this->qgamma = rlQGamma;
    EV << "RF alpha " << qalfa << " and Gamma " << qgamma << "\n";
    // Initialize the state and the action spaces pointers (not the values)
    ac.V = new double*[numSSs];         // stores state values, stream states (index is the metric related to distances)
    ac.avgR = new double*[numSSs];      // stores average state reward values
    ac.A = new double**[numSSs];        // stores preferences to select specific options
    ac.N = new long*[numSSs];          // stores visit counts
    Ct = new double*[numSSs];          // stores visit counts
    ac.stateNext = new int[numSSs];     // stores a value of up to MAX number of STATES for next state
    ac.stateCurrent = new int[numSSs];  // stores a value of up to MAX number of STATES for current state
    ac.lastAction = new int[numSSs];    // keeps the current action of the Q(s,a) per stream
    ac.actions = retxSlots + 1;         // number of possible actions (# of RETX slots or no retransmission)

    for (i = 0; i < numSSs; i++)
    {
        ac.stateNext[i] = 0;
        ac.stateCurrent[i] = 0;
        ac.askRetx[i] = 0;
        ac.lastAction[i] = 0;
    }

    // Call CAOS algorithm
    chaos = new caos(numSSs, tdmaSF, retxSlots);
}

/* Scheduler core based on a
 * method of REINFORCEMENT LEARNING */
void RLScheduler::allocate(policySch_t policy, ssRequest_t* rq, PktAlloc* pkt, mkFirm* stream, int simCnt)
{
    int txSlots = tdmaSF - retxSlots, nextId, chunks = 0, i, j;
    bool markedSSs[numSSs];
    bool foundNext = false;

    AC__init(stream);

    aPkt->init(pkt);

    chunks = AC(policy, pkt, stream, chunks, simCnt);

    /*
     * Allocate normal packets
     * */
    for (i = 0; i < numSSs; i++)
        markedSSs[i] = false;

    while (txSlots > 0)
    {
#ifdef WITH__AKAROA
        nextId = UniformInt(0, numSSs - 1);
#else
        nextId = intrand(numSSs);
#endif
        foundNext = false;
        // O(n) - search for next id to allocate
        for (i = nextId; i < numSSs + nextId; i++)
        {
            j = i % numSSs;
            if (rq[j].p && !markedSSs[j])
            {
                markedSSs[j] = true;
                foundNext = true;
                break;
            }
        }

        // allocate per found index
        if (foundNext && txSlots >= rq[j].p)
        {
            txSlots -= rq[j].p;
            chunks = aPkt->add(pkt, chunks, PTYPE_NORMAL, rq[j].t, j, rq[j].p);
            if (chunks >= maxChunks)
                break;
        }
        else // no resource (should not get here)
        {
            if (foundNext)
                ev << "No resources for normal slots!!\n";
            break;
        }
    }

    // Report allocation to Event LOG
    aPkt->report(pkt);
}

int RLScheduler::AC(policySch_t policy, PktAlloc* pkt, mkFirm* stream, int chunks, int simCnt)
{
    int tx, i, next_tx = 0;
    double tmp_pref;
    double rew_corr;
    int min_idx_ns = -1, min_idx_cs = -1;

    for (i = 0; i < numSSs; i++)
        corrupt_stream[i] = stream->getBadPkts(i) ? 1 : 0;  // an immediate failure in the current SF

    /* =========================================================== */
    /* = RL Main Loop ============================================ */
    /* =========================================================== */
    for (i = 0; i < numSSs; i++)
    {
        if (policy == POLICY_RLQ_LB2 || policy == POLICY_RL) //2 or 0
            if (!corrupt_stream[i])
                continue;

        // Find next state (for this point, stream distance)
        // TODO: metric changes here
        ac.stateNext[i] = stream->getStreamState(i);

        // Adjust exploration-exploitation factor
        // In our algorithm the exploration is suggested when tau => 0
        // average the reward of stream
        // tau = ln(nt(s,a))/Ct; Ct=max_a|Q(s,bmax)-Q(s,a)|
        double actdiff = 0.0, CT = 0.0;
        int h_max = 0, h;

        //find max action index h_max
        double h_max_val = -100000000L;
        for (h = 0; h < ac.actions; h++)
        {
            if (ac.A[i][cs[i]][h] > h_max_val)
            {
                h_max_val = ac.A[i][cs[i]][h];
                h_max = h;
            }
        }
        // find max_a|Q(s,bmax)-Q(s,a)|
        for (h = 0; h < ac.actions; h++)
        {
            actdiff = fabs(ac.A[i][cs[i]][h_max] - ac.A[i][cs[i]][h]);
            if (actdiff > CT)
                CT = actdiff;
        }

        Ct[i][cs[i]] = CT;

        if (ac.N[i][cs[i]] > 0 && Ct[i][cs[i]] != 0)
        {
            tau[i] = log(ac.N[i][cs[i]])/Ct[i][cs[i]];
            EV << i << ":beta(" << tau[i] << ") tau("<<1/tau[i]<< ") Nt(" <<ac.N[i][cs[i]]<<") Ct("<<Ct[i][cs[i]]<<")\n";
        }
        else
            EV << i << ":tau (" << tau[i] << "Nt or Ct are ZERO\n";

        // Simplify state writing
        cs[i] = ac.stateCurrent[i];
        ns[i] = ac.stateNext[i];

        if (cs[i] != ns[i])
            EV << i << "_state_changed: " << cs[i] << " -> " << ns[i] << "\n";

        // Generate new action (new compared to prev_tx)
        // choose a from s
        tx = AC__actionSelect(stream, i, cs[i]);

        // Limit the #RETX slots per SS
        //tx = (tx > slotsPerUser) ? slotsPerUser : tx;
        ac.askRetx[i] = tx;
    }

    // Trim to create feasible actions (to sum up to T at most)
    if (policy == POLICY_RL)
        AC__Trim(stream);
    else
        AC__Rank(stream, trim_rank);

    // Manage actions
    for (i = 0; i < numSSs; i++)
    {
        tx = ac.getRetx[i];

        if (tx) // execute action a
            chunks = aPkt->add(pkt, chunks, PTYPE_RETX, TTYPE_1_2, i, tx);

        // Update Actor (Preferences)
        tmp_pref = ac.A[i][cs[i]][tx];
        rew_corr = adj_rew_by_actions(cs[i], tx);

        if (policy == POLICY_RL)    // SARSA
        {
            next_tx = tx;
        }
        else    //Q-Learning
        {
            next_tx = AC__MaxSelect(i, ns[i]);
        }

        // Increment #times action visits (for Boltzmann GLIE policy)
        ac.N[i][cs[i]]++;

        // select a' (maxQ)
        ac.A[i][cs[i]][tx] = ac.A[i][cs[i]][tx] + qalfa * ((reward[i] + rew_corr) + (qgamma * ac.A[i][ns[i]][next_tx]) - ac.A[i][cs[i]][tx]);

        //DEBUG *****************************************

        if (tmp_pref != ac.A[i][cs[i]][tx])
            EV << i << " --- Q(" << cs[i] << "," << tx << ")=" << tmp_pref << " -> " << ac.A[i][cs[i]][tx] << " / penalized by "<< reward[i] + rew_corr << "\n";

        EV<< "Str#" << i << ":";
        for (int u = 0; u < ac.actions; u++)
        {
            double min = -100000000L;
            min_idx_cs = -1;
            for (int h = 0; h < ac.actions; h++)
            {
                if (ac.A[i][cs[i]][h] > min)
                {
                    min = ac.A[i][cs[i]][h];
                    min_idx_cs = h;
                }
            }

            min = -100000000L;
            min_idx_ns = -1;
            for (int h = 0; h < ac.actions; h++)
            {
                if (ac.A[i][ns[i]][h] > min)
                {
                    min = ac.A[i][ns[i]][h];
                    min_idx_ns = h;
                }
            }

            if (min_idx_cs != u)
                EV << "["<< Gibbs(i,cs[i],u)<< "] ";
            else
                EV << "{{"<<  Gibbs(i,cs[i],u) << "}} ";
        }

        EV << "\n";
        EV << "Maximal next state action: " << min_idx_ns << "\n";

        // ENDDEBUG ********************************************

        // Update current state
        ac.stateCurrent[i] = ns[i];
    }

    return chunks;
}

int RLScheduler::node_asked_slots(int idx)
{
    return ac.askRetx[idx];
}

double RLScheduler::adj_rew_by_actions(int state, int desired_action)
{
    // the idea behind reward adjustment is to find out in each state
    // to how many packets were 'desired' by the RL compared to how many
    // are really required. For each extra packet, the penalty should increase
    int diff = desired_action - state;

    // the diff is exactly the number of packets allocated more than required
    return diff <= 0 ? 0.0 : (double)pow(diff,2) * rewardSlots;
}

/* Statistics, for specified unit (idx) look at array of
 * states to see the development of preferences */
double RLScheduler::getHighestPrefAction(mkFirm* stream, int i, int state)
{
    double maxg = 0.0, gibbs = 0.0;
    //int maxi = 0;

    // find the max likelihood action from gibbs_list
    for (int a = 0; a < ac.actions; a++)
    {
        gibbs = ac.gibbs_list[i][a];
        if (maxg < gibbs)
        {
            maxg = gibbs;
            //maxi = a;
        }
    }

    return maxg;
}

double RLScheduler::getAvgValueFunc(int idx, int state)
{
    return ac.avgR[idx][state];
}

/* records the current rewards for streams*/
void RLScheduler::rewardUpdate(int idx, double reward)
{
    this->reward[idx] = reward;
}

/*
 * Actor Critic Module Functions
 * */
void RLScheduler::AC__init(mkFirm* stream)
{
    static bool inited = 0;

    // Managing ReTx actions in an array
    for (int i = 0; i < numSSs; i++)
    {
        ac.askRetx[i] = 0;
        ac.getRetx[i] = 0;
    }

    // Proceed only first time, then return
    if (!inited)
        inited = true;
    else
        return;

    //TODO: currently supports only constant (m,k) per stream during simulation

    /*TODO: update metric (of V space) is going for violation severity states,
            now it is state 0 (for < m) and then more k-m+1, total k-m+2 */

    int states;
    cs = new int[numSSs];
    ns = new int[numSSs];

    for (int i = 0; i < numSSs; i++)
    {
        states = stream->getStreamMaxState(i);
        ac.V[i] = new double[states];
        ac.avgR[i] = new double[states];
        ac.A[i] = new double*[states];
        ac.N[i] = new long[states];
        Ct[i] = new double[ac.actions];
        ac.gibbs_list[i] = new double[ac.actions];

        for (int j = 0; j < ac.actions; j++)
            ac.gibbs_list[i][j] = 0.0;

        for (int j = 0; j < states; j++)
        {
            ac.V[i][j] = 0.0;
            ac.avgR[i][j] = 0.0;
            ac.A[i][j] = new double[ac.actions];
            ac.N[i][j] = 0;
            Ct[i][j] = 0.0;

            for (int k = 0; k < ac.actions; k++)
            {
                ac.A[i][j][k] = 0; //0.1*k;
            }

            //ac.A[i][j][0] = 0.1;
        }

        cs[i] = 0;
        ns[i] = 0;

        ac.stateCurrent[i] = states-1;
        ac.stateNext[i] = states-1;
    }
}

void RLScheduler::AC__Rank(mkFirm* stream, double rank_factor)
{
    int j, residue = 0, tot = 0, i,c, min_rank_id;
    double min_rank, rank[numSSs], prio[numSSs];

    tot = 0;

    j = intrand(numSSs);

    // Count the total number of requested retransmissions
    for (i = j; i < numSSs + j; i++)
    {
        c = i % numSSs;

        tot += ac.askRetx[c];
        ac.getRetx[c] = ac.askRetx[c];
        prio[c] = stream->getStreamK(c) - stream->getPriority(c) + 1;
        rank[c] = prio[c] * pErr[c];
    }

    residue = tot - retxSlots;

    while (residue > 0)
    {
        min_rank = 10000000.0;
        min_rank_id = -1;

        for (i = j; i < numSSs + j; i++)
        {
            c = i % numSSs;
            if (ac.getRetx[c] > 0 && rank[c] <= min_rank)
            {
                min_rank = rank[c];
                min_rank_id = c;
            }
        }

        if (min_rank_id == -1)
            return;

        if (ac.getRetx[min_rank_id] > 0)
        {
            ac.getRetx[min_rank_id]--;
            residue--;
        }

        rank[min_rank_id] *= rank_factor;
    }

    for (i = 0; i < numSSs; i++)
        if (ac.askRetx[i])
            EV << "{n-"<< i << "}:"<<  ac.askRetx[i] << "  allowed " << ac.getRetx[i] << " rank "<< rank[i] <<"\n";
}

/* Define the right "social law" so agent can behave within the
 * allowed limits, so this 'allocation' generates the limits for actions * */
void RLScheduler::AC__OptRLRealloc(mkFirm* stream, caos* chaos)
{
    int i, tot = 0;
    double ranks[numSSs];
    int corrupt_stream[numSSs];

    for (i = 0; i < numSSs; i++)
    {
        tot += ac.getRetx[i];
        ranks[i] = (double)ac.getRetx[i]/(double)retxSlots;
        corrupt_stream[i] = stream->getBadPkts(i) ? 1 : 0;
    }

    chaos->findOptimalNs(corrupt_stream, estErrors);

    for (i = 0; i < numSSs; i++)
    {
        if (ranks[i] >= 1.0)    // saturated request
            ac.askRetx[i] = chaos->getOptNs(i);
        else
            ac.askRetx[i] = 0;
    }

    //todo: maybe if askretx sum > retxslots then start removing by packet the ones with lower stream state...
}

void RLScheduler::AC__Trim(mkFirm* stream)
{
    int i, tot = 0, residue = 0, lowest_pid;
    double lowest_per;

    // Count the total number of requested retransmissions
    for (i = 0; i < numSSs; i++)
    {
        tot += ac.askRetx[i];
        ac.getRetx[i] = ac.askRetx[i];
    }

    residue = tot - retxSlots;

    while (residue > 0)
    {
        // Find the lowest PER stream but first find the one that has allocation
        lowest_per = 1.0;
        lowest_pid = 0;

        for (i = 0; i < numSSs; i++)
        {
            if (ac.getRetx[i] > 0 && pErr[i] <= lowest_per)
            {
                lowest_per = pErr[i];
                lowest_pid = i;
            }
        }

        // Find all streams with lower priority (from the ones that have asked for allocations)
        // With lowest PER selected to be cut out there still could be lower priority streams than this
        // which might be beneficial to cut out first, also might depend on the difference in PER's between the discussed streams
        lowest_pid = getLowestPrio(stream, lowest_pid);

        EV << "Residue=" << residue << ", Removed 1 from stream " << lowest_pid << " with PER=" << pErr[lowest_pid] << "\n";

        if (ac.getRetx[lowest_pid] > 0)
        {
            ac.getRetx[lowest_pid]--;
            residue--;
        }
        else
        {
            EV << "BAD LOOP STATE! SHOULD NOT GET HERE !!!\n";
            return;
        }
    }
}

double RLScheduler::Gibbs(int stream, int state, int action)
{
    double nom, denom = 0;

    for (int b = 0; b < ac.actions; b++)
    {
        denom += exp(ac.A[stream][state][b] * tau[stream]);
    }
    // update action a preference value by GIBBS
    nom = exp(ac.A[stream][state][action] * tau[stream]);
    return denom ? nom/denom : 0;
}

// Returns an integer from 0 to Number of Action (packets to Transmit)
int RLScheduler::AC__MaxSelect(int i, int state)
{
    double val = 0.0, maxg = -100000000L;
    int maxi = 0;

    // find the max likelihood action from gibbs_list
    for (int a = 0; a < ac.actions; a++)
    {
        val = ac.A[i][state][a];
        if (maxg < val)
        {
            maxg = val;
            maxi = a;
        }
    }

    return maxi;
}

// Returns an integer from 0 to Number of Action (packets to Transmit)
int RLScheduler::AC__actionSelect(mkFirm* stream, int i, int state)
{
    double maxg = 0.0, gibbs = 0.0;
    int a, maxi = 0, ns = state;
    double ra, lccp, rccp;

    // find the max likelihood action from gibbs_list
    for (a = 0; a < ac.actions; a++)
    {
        ac.gibbs_list[i][a] = Gibbs(i, ns, a);
        gibbs = ac.gibbs_list[i][a];
        if (maxg < gibbs)
        {
            maxg = gibbs;
            maxi = a;
        }
    }

    ra = uniform(0.0,1.0,0);
    lccp = 0.0;   // accumulated probability
    rccp = ac.gibbs_list[i][0];
    for (a = 0; a < ac.actions; a++)
    {
        if (ra >= lccp && ra < rccp)
        {
            EV << i << ": largest Action = " << maxi << " select -> " << a << "tau("<< tau[i] << ")\n";
            return a;
        }

        if (a+1 < ac.actions)
        {
            lccp += ac.gibbs_list[i][a];
            rccp += ac.gibbs_list[i][a+1];
        }
    }

    EV << "Failed selection!\n";
    return 0;
}

/*
 * End of AC
 * */

void RLScheduler::recordError(int id, double estPe)
{
    pErr[id] = estPe;
}

/*
 * Calculates all streams priorities and returns the lowest
 * from ones that contain allocation and lower than the lowest PER stream priority
 * */
int RLScheduler::getLowestPrio(mkFirm* stream, int lper_pid)
{
    int nxt_prio, tmp_prio = stream->getPriority(lper_pid);
    int i,j, startId, id = lper_pid;

#ifdef WITH__AKAROA
    startId = UniformInt(0, numSSs - 1);
#else
    startId = intrand(numSSs);
#endif

    for (j = startId; j < numSSs + startId; j++) // look for the lowest priority in random
    {
        i = j % numSSs;

        if (i != lper_pid)
        {
            nxt_prio = stream->getPriority(i);

            // if requested TX for and lower than the lowest-PER priority
            // higher priority -> 0
            if (ac.getRetx[i] && nxt_prio > tmp_prio)
            {
                tmp_prio    = nxt_prio;
                id      = i;
                EV << "Found a lower priority " << tmp_prio << " for stream " << id << "!\n";
            }
        }
    }

    return id;
}


}; // namespace
