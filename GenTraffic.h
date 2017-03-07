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

#ifndef __GENTFX_H
#define __GENTFX_H
#include <omnetpp.h>
#ifdef WITH__AKAROA
#include <../../akaroa-2.7.13/include/akaroa.H>
#include <../../akaroa-2.7.13/include/akaroa/distributions.H>
#endif

namespace ncqrs {

#define LOAD_MODEL_CBR       0      /* Periodic streams */
#define LOAD_MODEL_PBP       1      /* Probabilistic model of bursts based on Poisson */

typedef enum
{
    PTYPE_NORMAL,      /* Normal transmissions */
    PTYPE_RETX,        /* Retransmissions */
    PTYPE_ILLEGAL,
}
msgTypes_t;

extern const char* pktTypeStr[];

typedef enum
{
    TTYPE_2_3,      /* defined in relation to (m,k)-firm deadlines, m=2 and k=3 here */
    TTYPE_1_2,
    TTYPE_3_4,
    TTYPE_10_15,
    TTYPE_13_15,
    TTYPE_9_11,
    TTYPE_INPUT,
    TTYPE_MANUAL,
    TTYPE_ILLEGAL,
}
trafficTypes_t;

typedef enum
{
    POLICY_RL = 0,                         /* 0 SARSA */
    POLICY_RLQ,                            /* 1 Q-Learning for all N streams all times */
    POLICY_RLQ_LB2,                        /* 2 Q-Learning for M (failing) out of N */
    POLICY_DBP,                            /* 3 Distance Based Priority */
    POLICY_CAOS,                           /* 4 Channel aware Optimal Scheduling Prod(1-pi^ni), i=1,..,k */
    POLICY_DBRR,                           /* 5 Random policy looking at deadlines */
    POLICY_CAOS_DBP,                       /* 6 Mix between DBP and CAOS */
}
policySch_t;

typedef enum
{
    POLICY_REAL,
    POLICY_VIRT
}
policyNature_t;

typedef struct
{
    double reduce;
    double violate;
    double reduceAll;
    double violateAll;
    double total;
}
rewards_t;

#define M_TTYPE_2_3     2
#define K_TTYPE_2_3     3

#define M_TTYPE_1_2     1
#define K_TTYPE_1_2     2

#define M_TTYPE_3_4     3
#define K_TTYPE_3_4     4

#define M_TTYPE_9_11    9
#define K_TTYPE_9_11    11

#define M_TTYPE_10_15   12
#define K_TTYPE_10_15   15

#define M_TTYPE_13_15   13
#define K_TTYPE_13_15   15

#define K_MAX           25

#define MAX_STATE       K_MAX /* make the state max more generic */

#define INFINITE_STREAM -1

typedef struct          /* emulated SS's request at the BS */
{
    bool fresh;         /* stream status: true - new, false - not anymore */
    int m, k;           /* (m,k)-firm setting */
    trafficTypes_t t;   /* type of traffic corresponding to (m,k) */
    int p;              /* packets per stream */
    int ttl;            /* time to live - expiry time of a stream */
}
ssRequest_t;

class GenTraffic
{
    private:
      int numSSs;
      int streamType;
      int streamM, streamK;
      int* streamMArray, *streamKArray;
      bool streamRnd;
      int streamTLMin;  /* minimum assigned to TTL, in SF units */
      int streamTLMax;  /* maximum assigned to TTL, in SF units */
      bool infTTL;      /* enable infinite stream */
      double* tfcLoad;
      ssRequest_t* reQuests;
    public:
      GenTraffic(int nodes);
      ~GenTraffic();
      void initialize(int, int, int, const char*, const char*, bool, int, int, bool, const char* );
      ssRequest_t* popRequests(void);
      double getLambda(int SS);
      int getPackets(int SS);
      int getTTL();
      trafficTypes_t getTrafficType(void);
      void setMK(int req);
      void clrRequests();
};

}; // namespace

#endif
