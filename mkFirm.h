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

#ifndef MKFIRM_H_
#define MKFIRM_H_

#include <omnetpp.h>
#include "GenTraffic.h"

namespace ncqrs {

typedef enum
{
    RECOVER_DFE,
    IMPROVE_SDFE,
    REDUCE_SDFE,
    STILL_GOOD,
    STILL_DFE,
    INVALID_DFE
}
sRecovery_t;

typedef struct
{
    int m;
    int k;
}
mkSetting_t;

typedef struct
{
    bool* elm;
    int nelm;
}
mkFifo_t;

typedef struct
{
    int prio;
    int id;
}
streamPrio_t;

class mkFirm
{
    private:
        int numSSs;
        int latency;
        int* badPkts;
        mkSetting_t* mkSet;
        mkFifo_t* sFifo;
        mkFifo_t* sTmpFifo;     // used in API for allocation purposes only (renewed every SF)
    public:
        mkFirm(int, int);
        ~mkFirm();
        virtual void initialize();

        /* stream (m,k)-FIFOs API */
        void setMK(int idx, int m, int k);
        void setBadPkts(int idx, int val);
        int getBadPkts(int idx);
        int getStreamK(int idx);
        int getStreamMaxState(int idx);
        int getStreamM(int idx);
        void createFIFO(int idx);
        bool isFIFO(int idx);
        bool isFIFObyObject(int idx, mkFifo_t* fifo);
        bool isFIFOElm(int idx, int elm);
        bool isFIFOElmbyObject(int idx, mkFifo_t* fifo, int elm);
        void removeFIFO(int idx);
        void adjustFIFO(int idx);
        void pushFIFO(int idx, bool pbf);              /* PBF = Packet Binary Feedback */
        bool popFIFO(int idx);
        int getDistDFE(int idx);
        int getTmpDistDFE(int idx);
        int getDistDFEByObject(int idx, mkFifo_t* sFifo);    /* get the distance from a dynamic failure event (DFE) */
        int getn1s(int idx);
        int getStreamState(int idx);
        int getPriority(int idx);   /* gets the Distance Based Priority */
        int getTmpPriority(int idx);
        int getPriorityByObject(int idx, mkFifo_t* fifo);
        /* Triggered by retransmission, finds the first bad PBF in
         * the FIFO(idx) and recovers it, returns 0-n for the filled gap index
         * could return -1 if not found (weird!) */
        int recoverPacket(int idx, policyNature_t p);
        int recoverTmpPacket(int idx);
        int recoverPacketByObj(int idx, policyNature_t p, mkFifo_t* fifo);
        void cloneFifo();
        double rewardTF(int idx);
};

} /* namespace rlsch */

#endif /* SS_H_ */
