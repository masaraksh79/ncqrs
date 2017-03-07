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

#ifndef __BS_H
#define __BS_H

#include <omnetpp.h>
#include <RFChan.h>
#include <GenTraffic.h>
#include <pktAlloc_m.h>

namespace ncqrs {

/**
 * Implements the Base Station
 * Base station is a complex entity that is the core of this simulation
 * It includes WRR, Traffic Generation, Reinforcement Learning and the Base Class
 * implementing a "smart" RL-Based allocation mechanism
 */
class BS : public cSimpleModule
{
  private:
    RFChan* rf;
    int numSSs;
    long tdmaTs;
    long tdmaSF;
    int maxChunks;                 /* # of chunks total */
    int retxChunks;                /* # of chunks for retransmit */
    int retxSlots;                 /* # of real retransmit packets possible in SF */
    int stateCurrent;
    policySch_t schedPolicy;
    long SFcnt;
    int offsetID;
    int actPrefID;
    int SConv_Startx;
    double SConv_UpFactor;
    double SConv_Crossp;
    int SConv_batch;
    double* vioRateBuffer;
    long int vioRateBufferLen;
    double vioRateMA;              /* moving average signal for viorate */
    double sAvgActionMA;           /* moving average signal for num of actions per agent on average */
    double vioRateFactor;          /* moving average history factor for viorate */
    cOutVector *vvec;
    rewards_t reward;
    /* Statistics */
    simsignal_t* numActPrefsSignal;
    cProperty *statPropertyActPref;
    simsignal_t* numBadRxSignal;
    cProperty *statPropertyRxBad;
    simsignal_t* badTSignal;
    cProperty *statVioRates;
    simsignal_t badAvgTSignal;
    simsignal_t badAvgPErr;
    simsignal_t* rlAvgValue;
    cProperty *statPropertyRlValue;
    simsignal_t sAvgAction;
  protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    virtual void bcastMessage(cMessage *msg);
    int getSSID(cMessage* msg);
    void envReward();
    void initStatistics();
};

}; // namespace

#endif
