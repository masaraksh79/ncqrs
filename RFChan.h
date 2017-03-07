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

#ifndef RFCHAN_H_
#define RFCHAN_H_

#include <omnetpp.h>
#ifdef WITH__AKAROA
#include <../../akaroa-2.7.13/include/akaroa.H>
#include <../../akaroa-2.7.13/include/akaroa/distributions.H>
#endif

namespace ncqrs {

typedef enum
{
    NODE_SS,
    NODE_BS,
}
node_t;

typedef enum
{
    STATIC_CHANNEL = 0,
    FSM_CHANNEL
}
channel_t;

typedef enum
{
    GOOD_STATE = 0,
    BAD_STATE
}
channel_state_t;

typedef struct  /* FSM model variables (per link) */
{
    channel_state_t state;
    double eB;          /* packet error rate of Bad and Good states */
    double eG;
    double pBB;         /* Sojourn probabilities */
    double pGG;
    double B;           /* Burstiness index */
}
fsm_chan_t;

typedef struct
{
    double a,b,c,d;
}
ge_matrix_t;


class RFChan
{
    private:
        channel_t chType;
        int numSSs;
        node_t unit;
        double pi0, pi1;
        int myID;
        double  pPERSS;           /* P(err) used by SS */
        double* pPERBS;           /* P(err) used by BS */
        fsm_chan_t fsmChanSS;     /* FSM model for SS link simulation */
    protected:
        bool fsmStepSS();
        bool fsmStepBS(int link);
    public:
        RFChan(int nodes, node_t t, int myID);
        virtual ~RFChan();
        void initRFChan(int chanType, const char* pPER,
                        double EH1, double eB, double B);                   /* global initialization routine */
        void initFSM4SS(double EH1, double eB, double B);                   /* SS FSM initialization routine */
        void initFSM4BS();                                                  /* BS FSM initialization routine */
        void channelStatic(node_t t, int link, cPacket* p);
        void channelFSM(node_t t, int link, cPacket* p);
        void channelRun(node_t t, int link, cPacket* p);
        double uplinkError();
        void setUplinkError(double val);
        double UniformRFChan(double a, double b);
};

} /* namespace rlsch */

#endif /* SS_H_ */
