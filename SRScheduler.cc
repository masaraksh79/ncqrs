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

#include "SRScheduler.h"

namespace ncqrs {

// models for retransmit allocation
// static model which has a constant ratio of retransmit packets per S_dfe

SRScheduler::SRScheduler(int nodes, int tdmaSF, int maxChunks, int retxChunks,  int retxSlots, int offsID, double expTau, double lin)
{
    this->numSSs = nodes;
    this->tdmaSF = tdmaSF;
    this->maxChunks = maxChunks;
    this->retxChunks = retxChunks;
    this->retxSlots = retxSlots;
    this->offsetID = offsID;
    this->expTau = expTau;
    this->lin = lin;
}
SRScheduler::~SRScheduler(){}

void SRScheduler::initialize()
{
    aPkt        = new pktConstruct(offsetID, maxChunks);
    badPkts     = new long[numSSs];
    estErrors   = new double[numSSs];

    tmpPER      = new double[numSSs];
    sPrio       = new streamPrio_t[numSSs];
    optNs       = new int[numSSs];

    corrupt_stream = new int[numSSs];

    caosFail    = 0;

    for (int i = 0; i < this->numSSs; i++)
    {
        this->badPkts[i] = 0;
        this->corrupt_stream[i] = 0;
        this->estErrors[i] = 0.0;
        this->optNs[i] = 0;

        // sPrio is used for distance based priority scheduling
        sPrio[i].prio = 0;
        sPrio[i].id = i;
    }

    // Call CAOS algorithm
    chaos = new caos(numSSs, tdmaSF, retxSlots);
}

/* allocate()
 * Scheduler based on definite policies: CADBP, DBP and DBRR
 * Splits J available slots into u*m and t where u*m+t=J
 * and u is the number of randomly picked users, t - preset
 * The m is fixed for a fixed super frame size
 * Last slot or y slots left not sufficient to meet the last deadline
 * are kept for retransmission
 *  */
void SRScheduler::allocate(policySch_t policy, ssRequest_t* rq, PktAlloc* pkt, mkFirm* stream)
{
    int txSlots = tdmaSF - retxSlots, nextId, chunks = 0, i, j;
    bool markedSSs[numSSs];
    bool foundNext = false;

    aPkt->init(pkt);

    /* Allocate retransmit slots */
    this->retransmitDBP(policy, pkt, &chunks, stream);

    /*
     * Allocate normal packets
     * */
    for (i = 0; i < numSSs; i++)
        markedSSs[i] = false;

    while (txSlots > 0)
    {
#ifdef WITH__AKAROA
        nextId = UniformInt(0, numSSs - 1);
#else
        nextId = intrand(numSSs);
#endif
        foundNext = false;
        // O(n) - search for next id to allocate
        for (i = nextId; i < numSSs + nextId; i++)
        {
            j = i % numSSs;
            if (rq[j].p && !markedSSs[j])
            {
                markedSSs[j] = true;
                foundNext = true;
                break;
            }
        }

        // allocate per found index
        if (foundNext && txSlots >= rq[j].p)
        {
            txSlots -= rq[j].p;
            chunks = aPkt->add(pkt, chunks, PTYPE_NORMAL, rq[j].t, j, rq[j].p);
            if (chunks >= maxChunks)
                break;
        }
        else // no resource (should not get here)
        {
            if (foundNext)
                ev << "No resources for normal slots!!\n";
            break;
        }
    }

    // Report allocation to Event LOG
    aPkt->report(pkt);
}

/*
 * Recalculates all streams priorities and returns the highest priority
 * */
int SRScheduler::getHighestPrio(mkFirm* stream, int * corrupt_stream)
{
    int hp = 255;

    for (int i = 0; i < numSSs; i++)
    {
        sPrio[i].prio = corrupt_stream[i] ? stream->getTmpPriority(i) : stream->getStreamK(i);
        sPrio[i].id = i;

        if (hp >= sPrio[i].prio)
            hp = sPrio[i].prio;
    }

    return hp;
}

/* retransmitDBP()
 * Allocate retransmissions using Distance Based Priority
 * */
void SRScheduler::retransmitDBP(policySch_t policy, PktAlloc* pkt, int *nextChunk, mkFirm* stream)
{
    int highestPrio, highestK = 0;
    int chunks = 0, i, j, r, retxPktCnt = 0;
    bool onePresent = true, resource = true;

    // Copy real streams into temporal ones (for assessment of improvement)
    stream->cloneFifo();

    for (i = 0; i < numSSs; i++)
        corrupt_stream[i] = stream->getBadPkts(i) ? 1 : 0;  // an immediate failure in the current SF

    //find the highest K to understand the priority span
    //TODO: still unsure what sorting later to perform to support different (m,k)-s.
    //      Probably require to keep highestK per stream and compare with it!?!

    // DBRR - select random streams who suffered from losses and allocate 1 pkt
    // until all pkts are gone or all streams are satisfied
    if (policy == POLICY_DBRR)
    {
        while (resource && onePresent)
        {
            onePresent = false;

#ifdef WITH__AKAROA
            r = UniformInt(0, numSSs - 1);
#else
            r = intrand(numSSs);
#endif

            for (i = r; i < numSSs + r; i++)
            {
                j = i % numSSs;

                // if distance less than k-m+1 then try to allocate
                if (corrupt_stream[j] &&
                    stream->getTmpDistDFE(j) < (stream->getStreamK(j) - stream->getStreamM(j) + 1) )
                {
                    if (retxPktCnt >= retxSlots)
                    {
                        ev << "Not enough slots\n";
                        resource = false;
                        break;
                    }

                   // allocate
                   chunks = aPkt->add(pkt, chunks, PTYPE_RETX, TTYPE_ILLEGAL, j, 1);
                   retxPktCnt++;
                   stream->recoverTmpPacket(j);
                   onePresent = true;
                }
            }
        }
    }

    else if (policy == POLICY_DBP)
    {
        for (i = 0; i < numSSs; i++)
        {
            if (!corrupt_stream[i])
                continue;

            if (highestK <= stream->getStreamK(i))
                highestK = stream->getStreamK(i);
        }

        // Retrieve all priorities (initially)
        // DBP1 or CA-DBP1
        highestPrio = getHighestPrio(stream, corrupt_stream);

        // Take the first batch of highest priorities and allocate for them
        while (resource && highestPrio < highestK)
        {
#ifdef WITH__AKAROA
            r = UniformInt(0, numSSs - 1);
#else
            r = rand() % numSSs;
#endif
            for (i = 0; i < numSSs; i++)
            {
                j = (i + r) % numSSs;   // a really random index

                // now seek from a random start for the highest priority
                if (highestPrio == sPrio[j].prio && corrupt_stream[j])
                {
                    // can't be more than retxSlots (number of actual RETX packets)
                    if (retxPktCnt >= retxSlots)
                    {
                        ev << "Not enough slots\n";
                        resource = false;
                        break;
                    }

                    // allocate
                    chunks = aPkt->add(pkt, chunks, PTYPE_RETX, TTYPE_ILLEGAL, sPrio[j].id, 1);
                    retxPktCnt++;

                    // general failure OR local bounding (for retransmit block)
                    if (chunks == -1 || retxChunks <= chunks)
                    {
                        ev << "Not enough chunks\n";
                        resource = false;
                        break;
                    }

                    // the allocation was a success, assume future recovery and adjust stream
                    // this is done on the virtual FIFO to affect virtual priority (virtual - only for allocation)
                    stream->recoverTmpPacket(sPrio[j].id);
                }
            }

            // Recalculate priorities
            // DBP1 or CA-DBP1
            highestPrio = getHighestPrio(stream, corrupt_stream);
        }
    }

    // For Channel Aware random one more packet for this alloc
    // So, we work on the left over channels from DBP work
    // Scanning through the streams, we find the one with highest PER
    // and allocate another packet to it if coin flip < 0.5+PER
    else if (policy == POLICY_CAOS || policy == POLICY_CAOS_DBP)
    {
        /* Optimality is by finding a set of n1,...,nk for k packets
         * that maximize the product F=(1-p1^n1)*...*(1-pk^nk)
         * This allocation maximizes the Lagrangian of the max(log(F)) problem iterating to find lambda
         * Then n_i's can be easily derived
         *
         * The inputs are   (1) retxSlots   (2) estErrors[]
         * Outputs are n's which are then directly allocated
         * N's are rounded so the sum can go above the max. allowed retransmission space
         * In this case the allocation runs up to the last alloted slot... */

        chaos->findOptimalNs(corrupt_stream, estErrors);

        for (i = 0; i < numSSs; i++)
            optNs[i] = chaos->getOptNs(i);

        // Fix coeffs
        if (policy == POLICY_CAOS_DBP)
        {
            double ranks[numSSs], gibbs[numSSs], fused[numSSs];
            int k, p, a, b;
            double  denom, nom;

            //Rounding the result back to optNs
            EV << "OptNs [ ";
            // update action a preference value by GIBBS
            for (a = 0; a < numSSs; a++)
                EV << optNs[a] <<" ";
            EV <<"]\n";

            EV << "Ranks [ ";
            for (int j = 0; j < numSSs; j++)
            {
                k = stream->getStreamK(j);
                p = k - stream->getPriority(j);
                ranks[j] = 2*p*p;
                EV << ranks[j] << " ";
            }

            EV <<"]\n";


            EV << "Gibbs [ ";
            // update action a preference value by GIBBS
            for (a = 0; a < numSSs; a++)
            {
                denom = 0.0;

                for (b = 0; b < numSSs; b++)
                    denom += exp(ranks[b] / expTau);

                nom = exp(ranks[a] / expTau);

                gibbs[a] = retxSlots * (denom ? nom/denom : 0);
                EV << gibbs[a] <<" ";
             }
             EV <<"]\n";

             //Doing linear superposition n = a*dbp+(1-a)*caos
             EV << "Fused [ ";
             // update action a preference value by GIBBS
             for (a = 0; a < numSSs; a++)
             {
                 fused[a] = lin*gibbs[a] + (1-lin)*(double)chaos->getOptNs(a);
                 EV << fused[a] <<" ";
             }
             EV <<"]\n";

             //Rounding the result back to optNs
             EV << "OptNs [ ";
             // update action a preference value by GIBBS
             for (a = 0; a < numSSs; a++)
             {
                 optNs[a] = round(fused[a]);
                 EV << optNs[a] <<" ";
             }
             EV <<"]\n";
        }

        for (i = 0; i < numSSs; i++)
        {
            // Allocation
            if (optNs[i] > 0)
            {
                if ((retxPktCnt + optNs[i]) <= retxSlots)
                {
                    chunks = aPkt->add(pkt, chunks, PTYPE_RETX, TTYPE_ILLEGAL, i, optNs[i]);
                    retxPktCnt += optNs[i];
                }
                else
                {
                    int add_pkts = retxPktCnt + optNs[i] - retxSlots;
                    if (add_pkts > 0)
                    {
                        chunks = aPkt->add(pkt, chunks, PTYPE_RETX, TTYPE_ILLEGAL, i, optNs[i] - add_pkts);
                        retxPktCnt += optNs[i] - add_pkts;
                    }
                    break;
                }
            }

            //ev << "Allocation(" << i << ") " << optNs[i] << " for stream PER = " << estErrors[i] << "\n";
        }
    }

    *nextChunk = chunks;
}

void SRScheduler::recordError(int id, double estPe)
{
    this->estErrors[id] = estPe;
}

}; // namespace
