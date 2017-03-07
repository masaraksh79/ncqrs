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

#include <SS.h>
#include <pktAlloc_m.h>
#include <RFChan.h>
#include <GenTraffic.h>

namespace ncqrs
{

Define_Module(SS);

msgTypes_t *msgType;
msgTypes_t *ev_retx, *ev_norm, *ev_expl;

void SS::finish()
{
    delete txPkt;
}

void SS::initialize()
{
    offsetID = par("offsetID").longValue();
    myID = getId() - offsetID;
    tdmaTs = par("tdmaTs").longValue();
    numSSs = par("numSSs").longValue();
    pPERch = par("pPERch").doubleValue();
    pPERtime = par("pPERtime").longValue();
    msgType = new msgTypes_t;
    txPkt = new cPacket;    // dummy pkt used for sending

    // use to switch packets on TxD
    ev_retx = new msgTypes_t;
    ev_norm = new msgTypes_t;

    *ev_norm = PTYPE_NORMAL;
    *ev_retx = PTYPE_RETX;

    if (myID < 0)
    {
        ev << "Problem with offset ID for SS !\n";
        return;
    }
    else
    {
        rf = new RFChan(numSSs, NODE_SS, myID);
        rf->initRFChan(par("chanType").longValue(), par("pPER"), par("EH1"), par("eB"), par("B"));
    }

    ev << "SS #" << getId() << " has joined, link error = " << rf->uplinkError() << "\n";

    txCnt = new int[PTYPE_ILLEGAL];
    for (int i = 0; i < PTYPE_ILLEGAL; i++)
        txCnt[i] = 0;
}

void SS::msgAttachCntxt(cMessage* msg, cMessage* next, bool isRFPkt)
{
    msgTypes_t* ptr = (msgTypes_t *)(msg->getContextPointer());

    if (ev_norm == ptr)
    {
        next->setContextPointer(ev_norm);

        if (isRFPkt && txCnt[PTYPE_NORMAL])
        {

            txPkt->setName("SSNormPkt");
            for (int i = 0; i < txCnt[PTYPE_NORMAL]; i++)
               sendSSPkt((cPacket *)next->dup(), i);

            txCnt[PTYPE_NORMAL] = 0;
        }

    }
    else if (ev_retx == ptr)
    {
        next->setContextPointer(ev_retx);

        if (isRFPkt && txCnt[PTYPE_RETX])
        {
            txPkt->setName("SSRetxPkt");
            for (int i = 0; i < txCnt[PTYPE_RETX]; i++)
               sendSSPkt((cPacket *)next->dup(), i);

            txCnt[PTYPE_RETX] = 0;
        }
    }
    else
       ev << "SS[" << myID << "] - failed to attach the packet type!";
}

void SS::msgSetCntxt(cMessage *msg, PktAlloc* pkt, int idx, int burst)
{
    char TxAlloc[15] = {0};
    msgTypes_t t = (msgTypes_t)pkt->getMsgType(idx);

    switch(t)
    {
        case PTYPE_NORMAL:
            msg->setContextPointer(ev_norm);
            sprintf(TxAlloc, "TxD %d Normal packets\n", burst);
            this->txCnt[PTYPE_NORMAL] = burst;
            break;
        case PTYPE_RETX:
            msg->setContextPointer(ev_retx);
            sprintf(TxAlloc, "TxD %d Retransmit packets\n", burst);
            this->txCnt[PTYPE_RETX] = burst;
            break;
        default:
            sprintf(TxAlloc, "Failed to find packet type!\n");
            ev << "SS[" << myID << "] - failed to create the packet type!";
            break;
    }

    bubble((const char *)TxAlloc);
}

void SS::sendSSPkt(cPacket *pkt, simtime_t delay)
{
    this->rf->channelRun(NODE_SS, myID, pkt);
    sendDelayed(pkt, delay, "out");
    //EV << myID << ": Tx @"<< simTime() << "\n";

    if (simTime() > this->pPERtime && this->pPERtime > 0)
    {
        this->rf->setUplinkError(this->pPERch);
    }
}

void SS::handleMessage(cMessage *msg)
{
    int n = 0;

    if (msg->isSelfMessage())
    {
        if (txCnt[PTYPE_NORMAL] || txCnt[PTYPE_RETX])
        {
            msgAttachCntxt(msg, (cMessage *)txPkt, true);
        }

        delete msg;
        return;
    }

    // Messages received from BS
    PktAlloc *pkt = (PktAlloc *)msg;
    pkt->setName("AllocBSPkt");

    // The SIM sets the packet bit if it fails to pass through the link
    // The receiver is expected here to check the bit and drop the packet
    // Packet Receive statistics aren't gathered in SS
    if (pkt->hasBitError())
    {
        delete pkt;
        bubble("Failed!");
        return;
    }

    int TsTx = 0;

    for (int i = 0; i < pkt->getSsNumber(); i++)
    {
        //ev << getId() <<" vs "<< pkt->getSsID(i) << " -> Scan in " << i << "\n";

        if (pkt->getSsID(i) == getId()) //Find my allocation
        {
            if (0 < (n = pkt->getSsSlots(i))) //Extract the number of packets to Tx
            {
                // Schedule the first packet
                // burst generator is held in txCnt
                cMessage* schedBurst = new cMessage;
                schedBurst->setName("SchedTxBurst");
                msgSetCntxt(schedBurst, pkt, i, n);
                scheduleAt(simTime() + TsTx, schedBurst);
            }
        }
        // Accumulate the slots allocated so far for the whole net and use for current node
        TsTx += pkt->getSsSlots(i);
    }

    delete pkt;
}

} /* namespace rlsch */
