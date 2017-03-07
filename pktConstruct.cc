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

#include <pktConstruct.h>

namespace ncqrs {

pktConstruct::pktConstruct(int offsetID, int maxChunks)
{
    this->offsetID = offsetID;
    this->maxChunks = maxChunks;
}
pktConstruct::~pktConstruct() {}

void pktConstruct::init(PktAlloc* pkt)
{
    pkt->setSsNumber(maxChunks);
    pkt->setMsgTypeArraySize(maxChunks);
    pkt->setSsIDArraySize(maxChunks);
    pkt->setTrafficTypeArraySize(maxChunks);
    pkt->setSsSlotsArraySize(maxChunks);

    for (int i = 0; i < maxChunks; i++)
    {
        pkt->setMsgType(i, PTYPE_ILLEGAL);
        pkt->setSsID(i, -1);
        pkt->setTrafficType(i, TTYPE_ILLEGAL);
        pkt->setSsSlots(i, 0);
    }
}

/* ALLOC packet add() method
 * Adds new data to an assigned chunk (by index chunk)
 * Prior to that, searching for an existing allocation (packet_type+id)
 * and if found the allocation is appended to it instead, to keep the amount of chunks used low
 * */
int pktConstruct::add(PktAlloc* pkt, int chunk, msgTypes_t ptype, trafficTypes_t ttype, int ssID, int pkts)
{
    if (chunk < maxChunks)
    {
        // Search for an existing chunk (by id and ttype) to minimize the number of chunks to at most numSSs
        for (int i = 0; i < maxChunks; i++)
        {
            if (ptype != PTYPE_ILLEGAL &&
                pkt->getSsID(i) == (ssID + offsetID) &&
                pkt->getMsgType(i) == ptype)
            {
                pkt->setSsSlots(i, pkt->getSsSlots(i) + pkts);
                EV << chunk << ": Appended " << pkts << " " << pktTypeStr[ptype] << " pkts to ID:"<< ssID << "\n";
                return chunk;
            }
        }

        pkt->setMsgType(chunk, ptype);
        pkt->setTrafficType(chunk, ttype);
        pkt->setSsID(chunk, ssID + offsetID);
        pkt->setSsSlots(chunk, pkts);

        if (PTYPE_RETX == ptype)
            EV << chunk << ": Allocated " << pkts << " " << pktTypeStr[ptype] << " pkts to ID:"<< ssID << "\n";
        return (chunk + 1);
    }

    return -1;
}

/*
 * Skim through the content of the packet and print a unified message
 * saying how many of each type were allocated
 * */
void pktConstruct::report(PktAlloc* pkt)
{
    int cnt_norm = 0, cnt_retx = 0;

    for (int i = 0; i < maxChunks; i++)
    {
        if (PTYPE_RETX == pkt->getMsgType(i))
            cnt_retx += pkt->getSsSlots(i);

        if (PTYPE_NORMAL == pkt->getMsgType(i))
            cnt_norm += pkt->getSsSlots(i);
    }

    EV << "Allocate RTX(" << cnt_retx << ") NORM(" << cnt_norm << ")\n";
}


} /* namespace rlsch */
