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

#ifndef __SR_SCHED_H
#define __SR_SCHED_H

#include <omnetpp.h>
#include "GenTraffic.h"
#include "pktAlloc_m.h"
#include "pktConstruct.h"
#include "mkFirm.h"
#include "caos.h"

namespace ncqrs {

class SRScheduler
{
    private:
        int numSSs;
        int tdmaSF;
        int maxChunks;
        int retxChunks;
        int retxSlots;
        int offsetID;
        double expTau;
        double lin;
        long* badPkts;
        double* estErrors;
        int* optNs;
        double* tmpPER;
        int* corrupt_stream;
        caos *chaos;

        pktConstruct *aPkt;
        streamPrio_t* sPrio;
        double LHS(double x, int*);
        double binarysearch(int* steps, double m, double n, int* corrupt_stream, double gap, double* err);
        double expsearch(int* steps, double gap, double* err, int* corrupt_stream);
        double Coeff_N(double x, int idx);
    public:
        int caosFail;

        SRScheduler(int nodes, int tdmaSF, int maxChunks, int retxChunks, int retxSlots, int offsID, double expTau, double lin);
        ~SRScheduler();
        virtual void initialize(void);
        virtual void allocate(policySch_t policy, ssRequest_t* rq, PktAlloc* pkt, mkFirm* stream);
        void retransmitDBP(policySch_t policy, PktAlloc* pkt, int *nextChunk, mkFirm* stream);
        void recordError(int id, double estPe);
        int getHighestPrio(mkFirm* stream, int * corrupt_stream);
        void findOptimalNs(int* corrupt_stream);
};

}; // namespace

#endif
