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

#include <RFChan.h>
#include <cstringtokenizer.h>

namespace ncqrs
{

/* Wrapped uniform() omnet func to be mixed with Akaroa mode */
double RFChan::UniformRFChan(double a, double b)
{
#ifdef WITH__AKAROA
    return Uniform(a, b);
#else
    return uniform(a, b, 0);
#endif
}

/* In SS the nodes are used to parse the pErr out of a string */
RFChan::RFChan(int nodes, node_t t, int myID)
{
    this->numSSs = nodes;
    this->unit = t;
    this->myID = myID;
}
RFChan::~RFChan(){}

/* Initialize channel models
 * =====================================================
 * Extract error string passed by simulation
 * STATIC_CHANNEL
 * Error probability is presented by Uniform(BOT,TOP)
 * The transmitter flips the coin to fail the packet
 * FSM_CHANNEL
 * Gilbert-Elliot like model with two states
 * Parameters generated from PER are eB, eG and
 * transition probabilities of the state machine
 * =====================================================
 * */
void RFChan::initRFChan(int chanType, const char* pPER, double EH1, double eB, double B)
{
    int i;

    this->chType = (channel_t)chanType;

    /* SS is search for its pair inside the probability passed args */
    if (NODE_SS == this->unit)
    {
        cStringTokenizer tokenizer(pPER);
        for (i = 0; i < this->numSSs && tokenizer.hasMoreTokens(); ++i)
        {
            if (this->myID == i)
            {
                pPERSS = atof(tokenizer.nextToken());
                break;
            }
            else
            {
                tokenizer.nextToken();
            }
        }

        if (chType == STATIC_CHANNEL)
        {
            //ev << "SS link error is " << pPERSS << "\n";
        }
        else
        {
            initFSM4SS(EH1, eB, B);
        }
    }
    else    // BS
    {
        /* BS is grabbing all REAL link probabilities into its arrays[SS] */
        pPERBS = new double[this->numSSs];
        cStringTokenizer tokenizer(pPER);
        for (i = 0; i < this->numSSs && tokenizer.hasMoreTokens(); ++i)
            pPERBS[i] = atof(tokenizer.nextToken());

        // FSM channel, initialization of N channels
        initFSM4BS();
    }
}

/* Initializing FSM state parameters (SS)
 * */
void RFChan::initFSM4SS(double EH1, double eB, double B)
{
    double EH0;

    if (&fsmChanSS)
    {
        fsmChanSS.state = GOOD_STATE;
        fsmChanSS.eB = eB;
        fsmChanSS.eG = 0;       /* override with assumption */
        fsmChanSS.pBB = 1 - (1 / EH1);
        fsmChanSS.B = B;

        /* Holding times E[H0] E[H1] */
        EH0 = EH1 / fsmChanSS.B;
        fsmChanSS.pGG = 1 - (1 / EH0);

        /* According to A. Willig - Scheduling streams with (m,k)-firm deadlines having different
         * importance over Markovian Channels */
        pi0 = (1 - fsmChanSS.pBB)/ (2 - (fsmChanSS.pGG + fsmChanSS.pBB));
        pi1 = (1 - fsmChanSS.pGG)/ (2 - (fsmChanSS.pGG + fsmChanSS.pBB));

        /* P* = P0 x pi0 + P1 x pi1 = P1 x p1
         * P1 = PER / p1 */
        if (pi1 != 0)
            fsmChanSS.eB = pPERSS / pi1;

        ev << "FSM " << this->myID << ": eB="<< fsmChanSS.eB << " eG=" << fsmChanSS.eG << " pGG=" <<  fsmChanSS.pGG << " pBB=" <<  fsmChanSS.pBB << "\n";
    }
    else
    {
        ev << "FSM structure failed to loaded\n";
    }
}

/* Initializing FSM state parameters (BS for links to all SSs)
 * */
void RFChan::initFSM4BS()
{
    //EV << "Downlink error generation disabled\n";
    return;
}

void RFChan::channelRun(node_t t, int link, cPacket* p)
{
    if (chType == STATIC_CHANNEL)
        channelStatic(t, link, p);
    else //FSM
        channelFSM(t, link, p);
}

double RFChan::uplinkError()
{
    return pPERSS;
}

void RFChan::setUplinkError(double val)
{
    pPERSS = val;
}

/* channelStatic()
 * Emulates static channel per Packet
 * The packet error bit is set according to the decision of the static model */
void RFChan::channelStatic(node_t t, int link, cPacket* p)
{
    bool DLErrors = false;

    if (NODE_BS == t)
    {
        if (DLErrors)   // for now, assume no errors on DL
        {
            // Fail the packet (Static Channel implementation)
            if (UniformRFChan(0, 1) > pPERBS[link])
                p->setBitError(false);
            else
                p->setBitError(true);
        }
    }
    else    // SS
    {
        // Fail the packet (Static Channel implementation)
        if (UniformRFChan(0, 1) > pPERSS)
            p->setBitError(false);
        else
            p->setBitError(true);
    }
}

bool RFChan::fsmStepSS()
{
    bool pktOk = true;

    switch (fsmChanSS.state)
    {
        case GOOD_STATE:
            if (UniformRFChan(0,1) > fsmChanSS.eG)
                pktOk = true;
            else
                pktOk = false;
            if (UniformRFChan(0,1) > fsmChanSS.pGG)
                fsmChanSS.state = BAD_STATE;
            break;
        case BAD_STATE:
            if (UniformRFChan(0,1) > fsmChanSS.eB)
                pktOk = true;
            else
                pktOk = false;
            if (UniformRFChan(0,1) > fsmChanSS.pBB)
                fsmChanSS.state = GOOD_STATE;
            break;
        break;
    }

    return pktOk;
}

bool RFChan::fsmStepBS(int link)
{
    // TODO: if want errors on DL, implement a model here
    return true;
}

/* channelFSM()
 * Emulates static channel per Packet
 * The packet error bit is set according to the decision of the static model */
void RFChan::channelFSM(node_t t, int link, cPacket* p)
{
    if (NODE_BS == t)
    {
        // Fail the packet (FSM implementation)
        // Always PER=0 for DL now, implement fsmStepBS otherwise
        if (fsmStepBS(link))
            p->setBitError(false);
        else
            p->setBitError(true);
    }
    else    // SS
    {
        // Fail the packet (Static Channel implementation)
        if (fsmStepSS())
            p->setBitError(false);
        else
            p->setBitError(true);
    }
}

} /* namespace rlsch */
