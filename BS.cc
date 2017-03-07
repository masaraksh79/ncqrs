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

#include <BS.h>
#include <SchedWrap.h>
#include <RFChan.h>

namespace ncqrs {

Define_Module(BS);

GenTraffic* tf;
SchedWrap* sc;
ssRequest_t *ssReq;
PktAlloc *alloc_pkt;

char statisticName[32];
char signalName[32];

void BS::bcastMessage(cMessage *msg)
{
    for (int i = 0; i < this->numSSs; i++)
    {
        cMessage *copy = msg->dup();
        this->rf->channelRun(NODE_BS, i, (cPacket *)copy);
        send(copy, "out", i);
    }
}

void BS::finish()
{
    long double Yn = 0.0, Mn = 0.0;
    int i, j, k, t, jj;
    int batch = SConv_batch;

    if (!SConv_Startx)
    {
        if (this->vioRateFactor > 0)
            recordScalar("vioAvgRate", vioRateMA / vioRateFactor);

        if (sc->timesCaosFailure())
            recordScalar("caosFailures", sc->timesCaosFailure());

        delete vioRateBuffer;
        return;
    }

    // Find all long-term average
    k = vioRateBufferLen;
    jj = SConv_Startx / tdmaSF;  // start measurement of convergence
    j = 0;
    for (i = j; i < k; i++)
    {
        Mn += vioRateBuffer[i];
    }

    Mn /= (k-j);

    // Find the up-limit from which the signal is considered converged
    SConv_Crossp = Mn * SConv_UpFactor;

    for (i = j; i < k; i+=batch)
    {
        if (i == j)
            continue;

        Yn = 0; // evaluate one batch size
        for (t = j; t < i; t++)
        {
            Yn += vioRateBuffer[t];
        }
        Yn /= (i-j);

        if (Yn >= SConv_Crossp)
        {
            recordScalar("convP", (i - jj - batch/2));
            break;
        }
    }

    if (this->vioRateFactor > 0)
        recordScalar("vioAvgRate", vioRateMA / vioRateFactor);

    delete vioRateBuffer;
}

void BS::initialize()
{
    char simDurStr[20] = {0};

    /* BS General Initialization */
    offsetID = par("offsetID").longValue();
    numSSs = par("numSSs").longValue();
    tdmaTs = par("tdmaTs").longValue();
    tdmaSF = par("tdmaSF").longValue();
    maxChunks = par("maxChunks").longValue();
    retxChunks = par("retxChunks").longValue();
    retxSlots = par("retxSlots").longValue();
    actPrefID = par("actPrefNodeID").longValue();
    stateCurrent = par("streamK").longValue();
    schedPolicy = (policySch_t)par("schedMethod").longValue();

    /* Scheme convergence params */
    SConv_Startx = par("conv_startx").longValue();
    SConv_Crossp = 0.0; //evaluated in finish()
    SConv_UpFactor = par("conv_upfac").doubleValue();
    SConv_batch = par("conv_batch").longValue();

    /* Static Channel */
    rf = new RFChan(numSSs, NODE_BS, 0);
    rf->initRFChan(par("chanType").longValue(), par("pPER"), 0, 0, 0);

    /* Traffic Generation Initialization */
    tf = new GenTraffic(numSSs);
    ssReq = new ssRequest_t[numSSs];
    tf->initialize(par("streamType").longValue(),
                   par("streamM").longValue(),
                   par("streamK").longValue(),
                   par("streamMArr"),               /* manual array of stream m's*/
                   par("streamKArr"),               /* manual array of stream m's*/
                   par("streamRnd").boolValue(),
                   par("streamTLMin").longValue(),
                   par("streamTLMax").longValue(),
                   par("infTTL").boolValue(),
                   par("tfcLoad"));

    /* Scheduler loading (load an abstract Scheduler named SchedWrap{} )*/
    sc = new SchedWrap(numSSs, tdmaSF, offsetID, maxChunks, retxChunks, retxSlots);
    sc->initialize(par("schedMethod").longValue(),        /* scheduler method */
                   par("pErrInitial").doubleValue(),      /* RL - initial known P_err */
                   par("pErrEWMA").doubleValue(),         /* A factor for Exponentially weighted MA */
                   par("rewardReduce").doubleValue(),     /* Two rewards for stream change of distance from violation */
                   par("rewardViolate").doubleValue(),
                   par("rewardSlots").doubleValue(),
                   par("rl_ac_alfa").doubleValue(),       /* (AC) Critic - value V(s) learning rate (the discount rate) */
                   par("rl_ac_beta").doubleValue(),       /* (AC) Actor - policy adjustment rate */
                   par("rl_ac_gamma").doubleValue(),      /* (AC) TD Error balance factor */
                   par("rl_q_alfa").doubleValue(),        /* (Q) TD Error balance factor */
                   par("rl_q_gamma").doubleValue(),       /* (Q) TD Error balance factor */
                   par("slots_per_usr").longValue(),      /* (RL) Slots per user */
                   par("trimRank").doubleValue(),         /* (RL) Packet trim rank balance */
                   par("latencySFs").longValue());        /* (m,k)-firm buffer systems fix latency */

    /* TODO: Pass rewards only to scheduler, so far got down to SchedWrap only! Needed by RL */
    reward.reduce = par("rewardReduce").doubleValue();
    reward.violate = par("rewardViolate").doubleValue();
    reward.total = 0;

    ev << "BS (#" << getId() << ") has started!\n";

    /* initialize a buffer for post-processing */
    strcpy(simDurStr, ev.getConfig()->getConfigValue("sim-time-limit"));

    /* violation rate manual stats gathering
     * omnetpp automated mean sums all simulation samples
     * this is why its not suitable for us when we wish to discard
     * too old samples for L << #simulations_SFs */
    this->vioRateMA = 0.0;
    this->vioRateFactor = par("maVioFactor").doubleValue();
    vioRateBufferLen = atol(strtok(simDurStr, "s")) / tdmaSF;
    vioRateBuffer = new double[vioRateBufferLen];
    for (int i = 0; i < vioRateBufferLen; i++)
        vioRateBuffer[i] = 0.0;

    /* Mechanism of bad/expected/good to account for
     * all packets statistics in a SF */
    sc->initSFPackets(true);

    ev << "Super Frame Size = " << tdmaSF << "[Ts]\n";
    alloc_pkt = new PktAlloc("ALLOC");
    scheduleAt(simTime() + 1, alloc_pkt);

    /* Register statistics related components */
    this->initStatistics();
}

void BS::initStatistics()
{
    int i;

    numActPrefsSignal   = new simsignal_t[MAX_STATE];   // max states are max_k + 1
    numBadRxSignal      = new simsignal_t[numSSs];
    badTSignal          = new simsignal_t[numSSs];
    rlAvgValue          = new simsignal_t[MAX_STATE];   // max states are max_k + 1

    statPropertyActPref = getProperties()->get("statisticRL", "actPrefs");
    statPropertyRlValue  = getProperties()->get("statisticSS", "rlAvgValueFunction");


    for (i = 0; i < MAX_STATE; i++)
    {
        sprintf(signalName, "numActPrefs%d", i);
        numActPrefsSignal[i] = registerSignal(signalName);
        memset(signalName, 0, 32);

        sprintf(statisticName, "numActPrefs%d", i);
        ev.addResultRecorders(this, numActPrefsSignal[i], statisticName, statPropertyActPref);
        memset(statisticName, 0, 32);

        sprintf(signalName, "rlAvgValue%d", i);
        rlAvgValue[i] = registerSignal(signalName);
        memset(signalName, 0, 32);

        sprintf(statisticName, "rlAvgValue%d", i);
        ev.addResultRecorders(this, rlAvgValue[i], statisticName, statPropertyRlValue);
        memset(statisticName, 0, 32);
    }

    statPropertyRxBad   = getProperties()->get("statisticSS", "rxBadPkts");
    statVioRates        = getProperties()->get("statisticSS", "vioRates");

    for (i = 0; i < numSSs; i++)
    {
        sprintf(signalName, "numBadRx%d", i);
        numBadRxSignal[i] = registerSignal(signalName);
        memset(signalName, 0, 32);

        sprintf(statisticName, "numBadRx%d", i);
        ev.addResultRecorders(this, numBadRxSignal[i], statisticName, statPropertyRxBad);
        memset(statisticName, 0, 32);

        sprintf(signalName, "vioRate%d", i);
        badTSignal[i] = registerSignal(signalName);
        memset(signalName, 0, 32);

        sprintf(statisticName, "vioRate%d", i);
        ev.addResultRecorders(this, badTSignal[i], statisticName, statVioRates);
        memset(statisticName, 0, 32);
    }

    sAvgAction = registerSignal("sAvgAction");
    badAvgTSignal = registerSignal("vioAvgRate");
    badAvgPErr = registerSignal("pErrAvg");
}

void BS::handleMessage(cMessage *msg)
{
    static int simCnt = 0;

    if (msg->isSelfMessage())
    {
        // ******************************************************
        // SUPERFRAME CONTROL BLOCK
        // ******************************************************
        // Count all packets, the received and the expected ones
        // Find out what packets were not received
        sc->initSFPackets(false);
        sc->clearSFPackets();

        // Emulate SSs' requests as the SS channel access slot is not in the SOW
        ssReq = tf->popRequests();

        sc->refreshStreams(ssReq);

        // Count rewards from last cycle
        envReward();

        // Count spent time
        sc->spentTime();

        sc->finalizeSpentTime();
        double avgAction = 0.0;
        for (int i = 0; i < numSSs; i++)
        {
           //emit(badTSignal[i], sc->getVioRate(i));
           //emit(badTSignal[i], sc->get_node_asked_slots(i));
           avgAction += sc->get_node_asked_slots(i);
        }

        // get vioRate for all (average)
        if (vioRateFactor > 0)
        {
            // Take the violation rate Moving Average
            vioRateMA = vioRateMA + sc->getAvgVioRate() - (vioRateMA / vioRateFactor);
            // unused
            sAvgActionMA = sAvgActionMA + (avgAction/numSSs) - (sAvgActionMA / vioRateFactor);
            //emit(badAvgTSignal, sc->getAvgVioRate());
        }
        else
        {
            emit(badAvgTSignal, sc->getAvgVioRate());
            //emit(sAvgAction, avgAction/numSSs);
            //emit(badAvgPErr, sc->getAvgEEEWMA());
        }

       if (simCnt < vioRateBufferLen)
	      vioRateBuffer[simCnt++] = sc->getAvgVioRate();

        EV << "VIORATE:" << vioRateMA / vioRateFactor << "\n";

#ifdef WITH__AKAROA
        AkObservation(sc->getAvgVioRate());
#endif

        // New SF Scheduling
        sc->allocate(ssReq, (PktAlloc *)msg, simCnt);

        // Transmit allocation frame
        bcastMessage(msg);
        // Schedule the next SF scheduling round
        scheduleAt(simTime() + tdmaSF, msg);

        //for (int k = 0; k < stateCurrent; k++)
        //   emit(numActPrefsSignal[k], sc->getHighestPrefAction(actPrefID, k));

        // ******************************************************
        // END of a SUPERFRAME CONTROL
        // ******************************************************
        return;
    }

    //emit(numTotRxSignal[getSSID(msg)], 1L);

    msgTypes_t rxt = *((msgTypes_t *)(msg->getContextPointer()));
    //ev << "Arrived type(" << rxt << ") from SS#" << getSSID(msg) << "\n";

    // BS receives allocated transmitted packets from SSs
    if (((cPacket *)msg)->hasBitError())
    {
        bubble("Failed!");
        //emit(numBadRxSignal[getSSID(msg)], 1L);
        sc->setBadPkt(getSSID(msg), rxt);
        delete msg;
        return;
    }
    else // good packet
    {
        if (rxt == PTYPE_RETX)
            sc->recoverPkt(getSSID(msg), POLICY_REAL);

        sc->setGoodPkt(getSSID(msg), rxt);
    }

    delete msg;
}

int BS::getSSID(cMessage* msg)
{
    int id = ((cPacket *)msg)->getSenderModuleId() - offsetID;
    return (id >=0) ? id : -1;
}

void BS::envReward()
{
    double outrew;
    // Check stream condition at the end of the SuperFrame
    // Allocate REWARDS to streams following next scheme:
    // If gone to Violation or can't recover from violation -> Assign 'reward.violate'
    // If in Violation and Recovered -> Assign 'reward.recover'
    // If good and Improved S_DFE -> Assign 'reward.improve'
    reward.reduceAll = 0.0;
    reward.violateAll = 0.0;

    for (int i = 0; i < numSSs; i++)
    {
        sc->rewardUpdate(i, 0);

        switch (sc->assignReward(i, &outrew))
        {
            case STILL_DFE:
                reward.violateAll += reward.violate * outrew;    // summing negative as absolute
                sc->rewardUpdate(i, reward.violate * outrew);
                break;
            case REDUCE_SDFE:
                reward.reduceAll += reward.reduce * outrew;    // summing negative as absolute
                sc->rewardUpdate(i, reward.reduce * outrew);
                break;
            case RECOVER_DFE:
            case IMPROVE_SDFE:
            case STILL_GOOD:
                break;
            default:
            case INVALID_DFE:
                EV << "Invalid value from reward assignment function !!!\n";
                break;
        }
    }

    /* summarized reward */
    reward.total = (reward.reduceAll + reward.violateAll);
    EV << "Reward accumulated total [R/V]: " << reward.total << " [" << reward.reduceAll
            << "/" << reward.violateAll << "]\n";

    // Record all stream S_DFE states at the beginning of next SF
    sc->captureStreams();
}

}; // namespace
