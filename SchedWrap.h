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

#ifndef __A_SCHED_H
#define __A_SCHED_H

#include <omnetpp.h>
#include "GenTraffic.h"
#include "RLScheduler.h"
#include "SRScheduler.h"
#include "pktAlloc_m.h"

namespace ncqrs {

typedef struct  /* Accounts for various possible realizations of a single SF */
{
    long *good;       /* good NORMAL packets received, per SS */
    long *exp;        /* expected according to allocation, per SS */
    long *bad;        /* NORMAL packets received with error, per SS */
    long *good_rtx;
    long *bad_rtx;
}
rxPkts_t;

class SchedWrap
{
    private:
        int numSSs;
        int tdmaSF;
        int maxChunks;
        int retxChunks;
        int retxSlots;
        int offsetID;
        int** spentState;
        double* vioFrac;
        double vioAvgFrac;
        int* sDFE;                     /* a local array of stream distance to failures */

        policySch_t schedMethod;       /* Select scheduler from a set of scheduler schemes */

        double* pEE;                   /* Estimated Probability of Error per link */
        double ewmaAlfa;               /* EWMA factor */

        /*
         * Reinforcement Learning
         * */

        RLScheduler* scRL;

        /*
         * Myopic scheduler (static)
         * */

        SRScheduler* scSR;

    public:
        SchedWrap(int nodes, int tdmaSF, int offsID, int maxChunks, int retxChunks, int retxSlots);
        ~SchedWrap();
        void initialize(int schMethod, double pErrInitial, double pErrEWMA,
                        int rewReduce, int rewViolate, double rewSlots,
                        double rlAlfa, double rlBeta, double rlGamma, double rlQAlfa, double rlQGamma,
                        int slotsPerUsr, double trimRank, int latencySFs);
        void refreshStreams(ssRequest_t* rq);
        void allocate(ssRequest_t* rq, PktAlloc* pkt, int simCnt);
        void updateExpPackets(PktAlloc *pkt);
        sRecovery_t assignReward(int idx, double* out_reward);
        double getHighestPrefAction(int idx, int state);
        double getAvgValueFunc(int idx, int state);
        void recoverPkt(int idx, policyNature_t p);
        void captureStreams();
        void rewardUpdate(int idx, double reward);
        /* Super frame oriented statistics
         * Good/Expected/Bad packets */
        void initSFPackets(bool firstTime);
        void clearSFPackets();
        void setGoodPkt(int id, msgTypes_t t);
        void setBadPkt(int id, msgTypes_t t);
        long* getBadPkts();
        int getBadStreamPkt(int idx);
        void setExpectedPkts(int id, int pkts);
        void updateEEEWMA(int idx, int pbf);
        double getAvgEEEWMA();
        void spentTime();
        double get_node_asked_slots(int i);
        double getAvgVioRate();
        double getVioRate(int i);
        void finalizeSpentTime();
        int timesCaosFailure();
};

}; // namespace

#endif
