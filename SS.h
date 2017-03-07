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

#ifndef SS_H_
#define SS_H_

#include <omnetpp.h>
#include <RFChan.h>
#include <pktAlloc_m.h>
#include <GenTraffic.h>

namespace ncqrs {

class SS : public cSimpleModule
{
    private:
        RFChan* rf;
        int offsetID;
        long tdmaTs;
        int numSSs;
        double pPERch;
        int pPERtime;
        int myID;
        int *txCnt;
        cPacket* txPkt;
    protected:
        virtual void initialize();
        virtual void finish();
        virtual void handleMessage(cMessage *msg);
        void sendSSPkt(cPacket *pkt, simtime_t delay);
        // Attaching proper packet type to the transmitted burst packets
        // Is also used to attach to next scheduled event in the burst - chaining the burst with the type
        void msgAttachCntxt(cMessage* msg, cMessage* next, bool isRFPkt);
        // Setting the burst scheduling with proper type (Allocated burst can only be of one type)
        void msgSetCntxt(cMessage *msg, PktAlloc* pkt, int idx, int burst);
};

} /* namespace rlsch */

#endif /* SS_H_ */
