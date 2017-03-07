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

#include <SchedWrap.h>
#include <mkFirm.h>

namespace ncqrs {

rxPkts_t *rxPkts;
mkFirm* stream;
bool *currStreams;

SchedWrap::SchedWrap(int nodes, int tdmaSF, int offsID, int maxChunks, int retxChunks, int retxSlots)
{
    this->numSSs = nodes;
    this->tdmaSF = tdmaSF;
    this->maxChunks = maxChunks;
    this->retxChunks = retxChunks;
    this->retxSlots = retxSlots;
    this->offsetID = offsID;
}
SchedWrap::~SchedWrap(){}

void SchedWrap::initialize(int schMethod, double pErrInitial, double pErrEWMA,
                           int rewReduce, int rewViolate,double rewSlots,
                           double rlAlfa, double rlBeta, double rlGamma,
                           double rlQAlfa, double rlQGamma,
                           int slotsPerUsr, double trimRank, int latencySFs)
{
    rxPkts = new rxPkts_t[numSSs];
    sDFE = new int[numSSs];
    spentState = new int*[numSSs];
    vioFrac = new double[numSSs];

    for (int i = 0; i < numSSs; i++)
    {
        spentState[i] = new int[MAX_STATE];
        for (int j = 0; j < MAX_STATE; j++)
            spentState[i][j] = 0;
    }

    /* Select scheduler to be used */
    this->schedMethod = (policySch_t)schMethod;

    switch(this->schedMethod)
    {
        case POLICY_RL:                         /* Reinforcement Learning */
            ev << "*** Q-Learning RL-CAOS Scheduler ***\n";
            break;
        case POLICY_RLQ:
            ev << "*** Q-Learning RL-trim-CAlpha Scheduler ***\n";
        case POLICY_RLQ_LB2:
            ev << "*** Q-Learning RL-trim-DynAlpha Scheduler ***\n";
            break;
        case POLICY_DBP:                        /* Distance Based Priority */
            ev << "*** Distance Based Priority (DBP1) Scheduler ***\n";
            break;
        case POLICY_CAOS:                      /* Channel Aware Optimal Scheduler */
            ev << "*** Channel Aware Optimal Scheduler ***\n";
            break;
        case POLICY_CAOS_DBP:                  /* Channel Aware Optimal Scheduler + DBP */
            ev << "*** Channel Aware and Priority Aware Optimal Scheduler ***\n";
            break;
        case POLICY_DBRR:                       /* Random policy looking at deadlines */
            ev << "*** Distance Based Random Recovery Scheduler ***\n";
            break;
    }

    /* Loaded all supported schedulers */
    /* REINFORCEMENT_LEARNING */
    scRL = new RLScheduler(numSSs, tdmaSF, maxChunks, retxChunks, retxSlots, offsetID);
    scRL->initialize(pErrInitial, rlAlfa, rlBeta, rlGamma, rlQAlfa, rlQGamma,
            slotsPerUsr, trimRank, rewSlots);    /* RL - initial known P_err */
    /* STATIC_RETRANSMISSION */
    scSR = new SRScheduler(numSSs, tdmaSF, maxChunks, retxChunks, retxSlots, offsetID,
                           1.0, 1.0);
    scSR->initialize();          /* SR - preset time slots for retransmission */

    /* initialize the estimated error vars and EWMA */
    this->pEE = new double[numSSs];
    this->ewmaAlfa = pErrEWMA;

    /* deadline handling mk-Firm module */
    stream = new mkFirm(numSSs, latencySFs);
    stream->initialize();

    currStreams = new bool[numSSs];
    for (int i = 0; i < numSSs; i++)
    {
        currStreams[i] = false;
        this->pEE[i] = pErrInitial;
    }
}

void SchedWrap::refreshStreams(ssRequest_t* rq)
{
    int i;

    for (i = 0; i < numSSs; i++)
    {
        if (rq[i].p && rq[i].fresh)    // new stream
        {
            currStreams[i] = true;
            stream->setMK(i, rq[i].m, rq[i].k);  // setting internal variables

            if (!stream->isFIFO(i))
            {
                ev << "Stream #" << i << " created, MK(" << rq[i].m << "," << rq[i].k << ")\n";
                stream->createFIFO(i);
                sDFE[i] = stream->getDistDFE(i);
            }
            else // adjusting size (will proceed if k changed)
            {
                ev << "Stream #" << i << " was adjusted, MK(" << rq[i].m << "," << rq[i].k << ")\n";
                stream->adjustFIFO(i);
            }
        }
    }

    // collect finished streams
    // only important when using stream random generation (SRG) load model
    for (i = 0; i < numSSs; i++)
    {
        if (currStreams[i] && rq[i].t == TTYPE_ILLEGAL)
        {
            stream->setMK(i, 0, 0);
            stream->removeFIFO(i);
            currStreams[i] = false;
            ev << "Stream #" << i << " was removed!\n";
        }
    }
}

/* allocate()
 * General allocation method, used as the scheduler object call */
void SchedWrap::allocate(ssRequest_t* rq, PktAlloc* pkt, int simCnt)
{
    switch(schedMethod)
    {
        case POLICY_CAOS:
        case POLICY_CAOS_DBP:
        case POLICY_DBP:
        case POLICY_DBRR:
            scSR->allocate(schedMethod, rq, pkt, stream);
            break;
        case POLICY_RL:
        case POLICY_RLQ:
        case POLICY_RLQ_LB2:
        default:
            scRL->allocate(schedMethod, rq, pkt, stream, simCnt);
            break;
    }

    updateExpPackets(pkt);
}

int SchedWrap::timesCaosFailure()
{
    if (schedMethod == POLICY_CAOS ||
        schedMethod == POLICY_CAOS_DBP)
        return scSR->caosFail;
    else
        return 0;
}

/* Count for each stream how much SFs the system spends in different states
 * Run this function every SF */
void SchedWrap::spentTime()
{
    for (int i = 0; i < numSSs; i++)
       this->spentState[i][stream->getStreamState(i)]++;
}

/* Calculate on finish() all the spent time in Good states
 * Good state (from k-1's) are 0,...,m-1*/
void SchedWrap::finalizeSpentTime()
{
    int badTime, totTime;

    vioAvgFrac = 0.0;

    for (int i = 0; i < numSSs; i++)
    {
        badTime = totTime = 0;

        for (int j = 0; j < stream->getStreamMaxState(i); j++)
        {
            totTime += this->spentState[i][j];
        }

        for (int j = stream->getStreamK(i) - stream->getStreamM(i) + 1; j < stream->getStreamMaxState(i); j++)
            badTime += this->spentState[i][j];

        vioFrac[i] = ((double)badTime / (double)totTime);

        vioAvgFrac += vioFrac[i];
    }

    vioAvgFrac /= numSSs;

    for (int i = 0; i < numSSs; i++)
        for (int j = 0; j < stream->getStreamMaxState(i); j++)
            this->spentState[i][j]=0;
}

double SchedWrap::get_node_asked_slots(int i)
{
    return scRL->node_asked_slots(i);
}

double SchedWrap::getAvgVioRate()
{
    return this->vioAvgFrac;
}

double SchedWrap::getVioRate(int i)
{
    return this->vioFrac[i];
}

/* Get preferences actions */
double SchedWrap::getHighestPrefAction(int idx, int state)
{
    switch(schedMethod)
    {
        case POLICY_RL:
        case POLICY_RLQ:
        case POLICY_RLQ_LB2:
            return scRL->getHighestPrefAction(stream, idx, state);
        default:
            return 0;
    }
}

/* Get value func */
double SchedWrap::getAvgValueFunc(int idx, int state)
{
    switch(schedMethod)
    {
        case POLICY_RL:
        case POLICY_RLQ:
        case POLICY_RLQ_LB2:
            return scRL->getAvgValueFunc(idx, state);
        default:
            return 0;
    }
}

sRecovery_t SchedWrap::assignReward(int idx, double* out_reward)
{
    int bfr, aft;
    //int m;

    if (idx < 0 || idx >= numSSs)
    {
        ev << "Bad index in recoverPkt!\n";
        return INVALID_DFE;
    }

    bfr = sDFE[idx];
    aft = stream->getDistDFE(idx);

    //EV << "Stream values: S_BFR " << bfr << " S_AFT " << aft << " M " << m << "\n";

    if (bfr && !aft)    // passed to violation
    {
        //EV << "Violated stream #" << idx << " S_dfe from " << bfr << "->" << aft << "\n";
        *out_reward = stream->rewardTF(idx);
        return STILL_DFE;
    }

    if (!bfr && !aft)     // didn't recover
    {
        //EV << "In violation stream #" << idx << " S_dfe from " << bfr << "->" << aft << "\n";
        *out_reward = stream->rewardTF(idx);
        return STILL_DFE;
    }

    // keeps above violation but below the best k-m+1
    if (aft > 0 && aft < (stream->getStreamK(idx)-stream->getStreamM(idx)+1))
    {
        //EV << "Degrading stream's #" << idx << " S_dfe from " << bfr << "->" << aft << "\n";
        *out_reward = stream->rewardTF(idx);
        return REDUCE_SDFE;
    }

    *out_reward = 0;
    return STILL_GOOD;
}

void SchedWrap::recoverPkt(int idx, policyNature_t p)
{
    stream->recoverPacket(idx, p);
}

void SchedWrap::captureStreams()
{
    for (int i = 0; i < numSSs; i++)
        sDFE[i] = stream->getDistDFE(i);
}

void SchedWrap::rewardUpdate(int idx, double reward)
{
    // Baseline schedulers don't use rewards as a feedback
    scRL->rewardUpdate(idx, reward);
}

/* Fill in packet content */
void SchedWrap::updateExpPackets(PktAlloc *pkt)
{
    int i, id, ap;

    for (i = 0; i < numSSs; i++)
        setExpectedPkts(i, 0);

    //Fill in the new allocation
    for (i = 0; i < pkt->getSsNumber(); i++)
    {
        id = pkt->getSsID(i) - offsetID;
        ap = pkt->getSsSlots(i);
        // parse expected IDs back to array[numSSs]
        setExpectedPkts(id, ap);
    }
}

void SchedWrap::initSFPackets(bool firstTime)
{
    // Statistics on what was on every SS in previous SF
    if (firstTime)
    {
        rxPkts->bad         = new long[numSSs];
        rxPkts->bad_rtx     = new long[numSSs];
        rxPkts->exp         = new long[numSSs];
        rxPkts->good        = new long[numSSs];
        rxPkts->good_rtx    = new long[numSSs];
    }
    else // ...usually
    {
        for (int i = 0; i < numSSs; i++)
        {
            if (rxPkts->exp[i] || rxPkts->good[i])
            {
                EV << "SS#" << i << " >>Exp:" <<rxPkts->exp[i]<< " [Good:"
                   << rxPkts->good[i] << " Retx:" << rxPkts->good_rtx[i]
                   << "] / [Bad:" << rxPkts->bad[i] << " Retx:" << rxPkts->bad_rtx[i]<< "] ^pe:"
                   << pEE[i] << " s_dfe:" << stream->getDistDFE(i) << " -- ";

                EV << "|";

                for (int j = 0; j < stream->getStreamK(i); j++)
                {
                    EV << stream->isFIFOElm(i,j) << "|";
                }

                EV << "\n";
            }
        }

    }
}

void SchedWrap::clearSFPackets()
{
    for (int i = 0; i < numSSs; i++)
    {
        rxPkts->bad[i]      = 0;
        rxPkts->bad_rtx[i]  = 0;
        rxPkts->exp[i]      = 0;
        rxPkts->good[i]     = 0;
        rxPkts->good_rtx[i] = 0;
    }
}

/* API for updating the packet statistics
 * goodSFPacket() and badSFPacket() - to be used directly inside the BS's receiver
 * expSFPacket() - to be used by scheduler, packets allocated 'de-facto' added per SS here */
void SchedWrap::setGoodPkt(int id, msgTypes_t t)
{
    updateEEEWMA(id, 0);    // EWMA of incoming error probabilities

    switch(schedMethod)
    {
        case POLICY_CAOS:
        case POLICY_CAOS_DBP:
        case POLICY_DBP:
        case POLICY_DBRR:
            scSR->recordError(id, pEE[id]);
            break;
        case POLICY_RL:
        case POLICY_RLQ:
        case POLICY_RLQ_LB2:
            scRL->recordError(id, pEE[id]);
            break;
        default:
            break;
    }

    if (id >=0)
    {
        if (t == PTYPE_NORMAL)
        {
            rxPkts->good[id]++;
            stream->setBadPkts(id, 0);
        }
        else
        {
            rxPkts->good_rtx[id]++;
            return;
        }
    }

    // push this PBF into the FIFO
    if (!stream->isFIFO(id))
    {
        ev << "BS [Error #1!] - no FIFO for arriving PBFs on " << id << "\n";
        return;
    }

    if (t == PTYPE_NORMAL)
        stream->pushFIFO(id, 1);
}

void SchedWrap::setBadPkt(int id, msgTypes_t t)
{
    updateEEEWMA(id, 1);    // EWMA of incoming error probabilities

    switch(schedMethod)
    {
        case POLICY_CAOS:
        case POLICY_CAOS_DBP:
        case POLICY_DBP:
        case POLICY_DBRR:
            scSR->recordError(id, pEE[id]);
            break;
        case POLICY_RL:
        case POLICY_RLQ:
        case POLICY_RLQ_LB2:
            scRL->recordError(id, pEE[id]);
        default:
            //TODO: record error
            break;
    }

    if (id >=0)
    {
        // TODO: maybe these counters are redundant to recordBadPkt?
        if (t == PTYPE_NORMAL)
        {
            rxPkts->bad[id]++;
            stream->setBadPkts(id, rxPkts->bad[id]);    //usable by CAOS scheduler
        }
        else
        {
            rxPkts->bad_rtx[id]++;
            return;
        }
    }

    // push this PBF into the FIFO
    if (!stream->isFIFO(id))
    {
        ev << "BS [Error #2!] - no FIFO for arriving PBFs on " << id << "\n";
        return;
    }

    if (t == PTYPE_NORMAL)
        stream->pushFIFO(id, 0);
}

long* SchedWrap::getBadPkts()
{
    return rxPkts->bad;
}

int SchedWrap::getBadStreamPkt(int idx)
{
    return stream->getBadPkts(idx);
}

void SchedWrap::setExpectedPkts(int id, int pkts) { if (id >=0) rxPkts->exp[id] = pkts; }

/* Estimated Error EWMA
 * */
void SchedWrap::updateEEEWMA(int idx, int pbf)
{
    pEE[idx] = (1 - ewmaAlfa) * pEE[idx] + ewmaAlfa * (double)pbf;
}

double SchedWrap::getAvgEEEWMA()
{
    double err = 0.0;

    for (int i = 0; i < numSSs; i++)
        err += pEE[i];

    return (err / numSSs);
}

}; // namespace
