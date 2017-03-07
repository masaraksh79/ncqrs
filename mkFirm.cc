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

#include <mkFirm.h>

namespace ncqrs {

mkFirm::mkFirm(int nodes, int latencySFs)
{
    numSSs = nodes;
    latency = latencySFs;
}
mkFirm::~mkFirm()
{
    delete mkSet;
    delete sFifo;
}

/* Recording the normal data received badly
 * at the immediate cycle
 * Needed for disciplines which aren't aware of (m,k)-firm */
void mkFirm::setBadPkts(int idx, int val)
{
    badPkts[idx] = val;
}

int mkFirm::getBadPkts(int idx)
{
    return badPkts[idx];
}

void mkFirm::initialize()
{
    mkSet = new mkSetting_t[numSSs];
    sFifo = new mkFifo_t[numSSs];
    sTmpFifo = new mkFifo_t[numSSs];
    badPkts = new int[numSSs];

    for (int i = 0; i < numSSs; i++)
    {
        this->setMK(i, 0, 0);
        sFifo[i].nelm = 0;
        sTmpFifo[i].nelm = 0;
        badPkts[i] = 0;
    }
}

void mkFirm::setMK(int idx, int m, int k)
{
    mkSet[idx].m = m;
    mkSet[idx].k = k;
}

void mkFirm::createFIFO(int idx)
{
    sFifo[idx].nelm = mkSet[idx].k;
    sFifo[idx].elm = new bool[mkSet[idx].k];

    // sTmpFifo is used for allocation only
    sTmpFifo[idx].nelm = mkSet[idx].k;
    sTmpFifo[idx].elm = new bool[mkSet[idx].k];

    for (int i = 0; i < mkSet[idx].k; i++)
        sFifo[idx].elm[i] = true;

}

bool mkFirm::isFIFO(int idx)
{
    return this->isFIFObyObject(idx, sFifo);
}

bool mkFirm::isFIFObyObject(int idx, mkFifo_t* fifo)
{
    return (fifo[idx].nelm > 0);
}

bool mkFirm::isFIFOElm(int idx, int elm)
{
    return this->isFIFOElmbyObject(idx, sFifo, elm);
}

bool mkFirm::isFIFOElmbyObject(int idx, mkFifo_t* fifo, int elm)
{
    return fifo[idx].elm[elm];
}

void mkFirm::removeFIFO(int idx)
{
    sFifo[idx].nelm = 0;
    delete sFifo[idx].elm;
}

void mkFirm::pushFIFO(int idx, bool pbf)
{
    for (int i = 0; i < sFifo[idx].nelm - 1; i++)
        sFifo[idx].elm[i] = sFifo[idx].elm[i+1];

    sFifo[idx].elm[sFifo[idx].nelm - 1] = pbf;
}

bool mkFirm::popFIFO(int idx)
{
    bool res = sFifo[idx].elm[0];
    for (int i = 0; i < sFifo[idx].nelm - 1; i++)
        sFifo[idx].elm[i] = sFifo[idx].elm[i+1];

    return res;
}

void mkFirm::adjustFIFO(int idx)
{
    int new_k = mkSet[idx].k;
    int old_k = sFifo[idx].nelm;

    // Case new_k > old_k
    // Newer FIFO size [1][2][3][4][5]
    // Older FIFO size [1][2]
    // Moving Older [1][2] to sit at [4][5]
    // Newer [1][2][3] marked as true (good PBFs)
    if (new_k > old_k)
    {
        bool backup[old_k];
        // backup older elements
        for (int i = 0; i < old_k; i++)
            backup[i] = popFIFO(idx);

        removeFIFO(idx);
        createFIFO(idx);
        for (int i = 0; i < old_k; i++)
            pushFIFO(idx, backup[i]);
    }
    // Case new_k < old_k
    // Newer FIFO size [1][2][3]
    // Older FIFO size [1][2][3][4][5]
    // Dump old_k - new_k from Older FIFO
    // Moving Older [3][4][5] to sit at Newer FIFO
    else if (old_k > new_k)
    {
        bool backup[old_k];

        // dump (old_k - new_k)
        for (int i = 0; i < old_k - new_k; i++)
            (void)popFIFO(idx);
        // backup older elements (new_k)
        for (int i = 0; i < new_k; i++)
            backup[i] = popFIFO(idx);

        removeFIFO(idx);
        createFIFO(idx);
        for (int i = 0; i < new_k; i++)
            pushFIFO(idx, backup[i]);
    }

    // no adjustment being made if old and new (m,k) sets are the same
}

int mkFirm::getDistDFE(int idx)
{
    return this->getDistDFEByObject(idx, sFifo);
}

int mkFirm::getTmpDistDFE(int idx)
{
    return this->getDistDFEByObject(idx, sTmpFifo);
}

int mkFirm::getDistDFEByObject(int idx, mkFifo_t* sFifo)
{
    int good = 0, distDFE = 0;

    if (mkSet[idx].k != sFifo[idx].nelm)
    {
        ev << "Error! Improper (m,k) set is applied on the current FIFO\n";
        return 0;
    }

    for (int i = 0; i < sFifo[idx].nelm; i++)
        if (sFifo[idx].elm[i])
            good++;

    distDFE = good - mkSet[idx].m + 1;

    return (distDFE < 0) ? 0 : distDFE;
}

int mkFirm::getn1s(int idx)
{
    int good = 0;

    for (int i = 0; i < sFifo[idx].nelm; i++)
        if (sFifo[idx].elm[i])
            good++;

    return good;
}

int mkFirm::getStreamState(int idx)
{
    return (mkSet[idx].k - getn1s(idx));
}

int mkFirm::getPriority(int idx)
{
    return this->getPriorityByObject(idx, sFifo);
}

int mkFirm::getTmpPriority(int idx)
{
    return this->getPriorityByObject(idx, sTmpFifo);
}

int mkFirm::getPriorityByObject(int idx, mkFifo_t* fifo)
{
    int i, n1s = 0, dist = 0, prio, meets = 0;

    // count number of successful 1s
    for (i = 0; i < fifo[idx].nelm; i++)
        n1s += fifo[idx].elm[i] ? 1 : 0;

    if (n1s == mkSet[idx].k)
    {
        dist = 1;
    }
    else if (n1s < mkSet[idx].m)
    {
        dist = mkSet[idx].k + 1;    // violation
    }
    else // find location (from right) of m-th meet (or '1')
    {
        dist = 1;
        // in the k-tuple of stream_idx the place 0 is i-1
        // so, the location should be inverted
        for (i = fifo[idx].nelm - 1; i >= 0 ; i--)
        {
            if (fifo[idx].elm[i])
            {
                if (++meets == mkSet[idx].m)
                {
                    dist = fifo[idx].nelm - i;
                    break;
                }
            }
        }
    }

    // convert distance to priority (k is the lowest priority)
    prio = mkSet[idx].k - dist + 1;

    if (prio < mkSet[idx].k)
    {
        dist = this->getTmpDistDFE(idx);
        //EV << "Prio " << prio << " stream #" << idx << " dist " << dist << " stream [";
        //for (int k = 0; k < mkSet[idx].k; k++)
        //    EV << fifo[idx].elm[k];
        //EV << "]\n";
    }
    return prio;
}

void mkFirm::cloneFifo()
{
    for (int i = 0; i < numSSs; i++)
    {
        sTmpFifo[i].nelm = sFifo[i].nelm;
        for (int j = 0; j < sTmpFifo[i].nelm; j++)
            sTmpFifo[i].elm[j] = sFifo[i].elm[j];
    }
}

int mkFirm::getStreamK(int idx)
{
    return mkSet[idx].k;
}

int mkFirm::getStreamMaxState(int idx)
{
    return mkSet[idx].k + 1;
}

int mkFirm::getStreamM(int idx)
{
    return mkSet[idx].m;
}

int mkFirm::recoverPacket(int idx, policyNature_t p)
{
    return this->recoverPacketByObj(idx, p, sFifo);
}

int mkFirm::recoverTmpPacket(int idx)
{
    return this->recoverPacketByObj(idx, POLICY_VIRT,sTmpFifo);
}

int mkFirm::recoverPacketByObj(int idx, policyNature_t p, mkFifo_t* fifo)
{
    if (!isFIFObyObject(idx, fifo))
    {
        ev << "Failed in recoverPacket - no FIFO!\n";
        return -1;
    }

    // due to latency restriction can recover only the recent packet
    // if the previous were not recovered in previous SFs then they're lost
    if (POLICY_REAL == p)
    {
        fifo[idx].elm[fifo[idx].nelm-1] = true;
    }
    else // Virtual recovery (used by DBP for virtual recovery algorithm)
    {
        if (latency > fifo[idx].nelm)
            latency = fifo[idx].nelm;

        for (int i = fifo[idx].nelm - latency; i < fifo[idx].nelm; i++)
        {
            if (!fifo[idx].elm[i]) // looking for zeroes
            {
                fifo[idx].elm[i] = true;
                return i;
            }
        }
    }

    return -1;
}

/* Reward Transfer Function (r = (k-1's)^2)
 * */
double mkFirm::rewardTF(int idx)
{
    int n1s = this->getn1s(idx);
    return (double)(pow(this->getStreamK(idx) - n1s, 2));
}

} /* namespace rlsch */
