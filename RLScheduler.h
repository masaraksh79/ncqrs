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

#ifndef __RL_SCHED_H
#define __RL_SCHED_H

#include <omnetpp.h>
#include "GenTraffic.h"
#include "pktAlloc_m.h"
#include "pktConstruct.h"
#include "mkFirm.h"
#include "caos.h"

namespace ncqrs {

typedef struct
{
    double **V;
    long **N;              // counts the number of times action visited in Q(s,a)
    double ***A;
    double **avgR;
    double **gibbs_list;
    int* stateNext;
    int* lastAction;
    int* stateCurrent;
    int actions;
    int* askRetx;           // how many RETX packets were requested by user
    int* getRetx;           // how many RETX packets were received by user
    double* lbCoeff;
}
ac_module_t;

class RLScheduler
{
    private:
        int numSSs;
        double* tau;
        double rewardSlots;
        int tdmaSF;
        int maxChunks;
        int retxChunks;
        int retxSlots;
        int offsetID;
        int *cs, *ns;       // state temp variables
        double **Ct;        // GLIE max|Qmax-Q| component
        long* badPkts;
        int slotsPerUser;
        double* reward;
        double trim_rank;
        double* estErrors;
        int* reqTx;                 /* Requested retransmissions */
        // Actor-Critic Section
        double alfa, beta, gamma, qalfa, qgamma;
        double* td_error;
        int* corrupt_stream;
        ac_module_t ac;
        caos *chaos;

        pktConstruct* aPkt;
        /*
         * Reinforcement Learning
         * */
        double *pErr;

        /* DBP elements in RL */
        streamPrio_t* sPrio;
    public:
        RLScheduler(int nodes, int tdmaSF, int maxChunks, int retxChunks, int retxSlots, int offsID);
        ~RLScheduler();
        virtual void initialize(double pErrInitial, double rlAlfa, double rlBeta,
                                double rlGamma, double rlQAlfa, double rlQGamma,
                                int slotsPerUsr, double trimRank, double rewSlots);
        virtual void allocate(policySch_t policy, ssRequest_t* rq, PktAlloc* pkt, mkFirm* stream, int simCnt);
        void rewardUpdate(int idx, double reward);
        void recordError(int id, double estPe);
        // Actor-Critic Section
        void    AC__init(mkFirm* stream);
        int     AC__actionSelect(mkFirm* stream, int i, int state);
        int     AC__MaxSelect(int i, int state);
        double  AC__CtEval(int i, int state);
        void    AC__OptRLRealloc(mkFirm* stream, caos* chaos);
        void    AC__Rank(mkFirm* stream, double rank_factor);
        void    AC__Trim(mkFirm* stream);
        int     AC(policySch_t policy, PktAlloc* pkt, mkFirm* stream, int chunks, int simCnt);
        double  Gibbs(int stream, int state, int action);
        double  getHighestPrefAction(mkFirm* stream, int idx, int state);
        double  getAvgValueFunc(int idx, int state);
        double  adj_rew_by_actions(int state, int desired_action);
        int     node_asked_slots(int idx);
        // DBP elements in RL
        int     getLowestPrio(mkFirm* stream, int lper_pid);
        int     findLowestPER(bool* marked);
};

}; // namespace

#endif
