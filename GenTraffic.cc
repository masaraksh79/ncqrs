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

#include <GenTraffic.h>
#include <cstringtokenizer.h>

namespace ncqrs {

const char* pktTypeStr[] = {"NORMAL", "RETX", "EXPL"};


GenTraffic::GenTraffic(int nodes) { this->numSSs = nodes; }
GenTraffic::~GenTraffic(){ delete this->reQuests; }

void GenTraffic::initialize(int streamType, int streamM, int streamK,
                            const char* streamMArr, const char* streamKArr,
                            bool streamRnd, int streamTLMin, int streamTLMax,
                            bool infTTL, const char* tfcLoad)
{
    // select load model
    this->streamType = streamType;
    if (LOAD_MODEL_CBR == this->streamType)
        ev << "*** Load model: Constant BitRate ***\n";
    if (LOAD_MODEL_PBP == this->streamType)
        ev << "*** Load model: Probabilistic Bursty Poisson ***\n";

    // set duration limits
    this->streamTLMin = streamTLMin;
    this->streamTLMax = streamTLMax;
    this->infTTL = infTTL;

    // set stream (m,k)
    this->streamM = streamM;
    this->streamK = streamK;
    this->streamRnd = streamRnd;

    // set stream arrays (will override the previous if set)
    this->streamMArray = new int[numSSs];
    this->streamKArray = new int[numSSs];

    for (int i = 0; i < numSSs; i++)
    {
        this->streamMArray[i] = MAX_STATE;
        this->streamKArray[i] = MAX_STATE;
    }

    // parse out and set stream load parameter (depends on load model)
    this->tfcLoad = new double[numSSs];
    cStringTokenizer tokenizer(tfcLoad);
    for (int i = 0; i < this->numSSs && tokenizer.hasMoreTokens(); ++i)
        this->tfcLoad[i] = atof(tokenizer.nextToken());

    // parse the m-s of the (m,k)-firm array
    cStringTokenizer tokenizer1(streamMArr);
    for (int i = 0; i < this->numSSs && tokenizer1.hasMoreTokens(); ++i)
        this->streamMArray[i] = atof(tokenizer1.nextToken());

    // parse the k-s of the (m,k)-firm array
    cStringTokenizer tokenizer2(streamKArr);
    for (int i = 0; i < this->numSSs && tokenizer2.hasMoreTokens(); ++i)
        this->streamKArray[i] = atof(tokenizer2.nextToken());

    // popRequests will initialize and generate requests, no work here
    this->reQuests = new ssRequest_t[this->numSSs];
    clrRequests();
}

void GenTraffic::clrRequests()
{
    for (int i = 0; i < this->numSSs; i++)
    {
        this->reQuests[i].fresh = false;
        this->reQuests[i].t = TTYPE_ILLEGAL;
        this->reQuests[i].m = 0;
        this->reQuests[i].k = 0;
        this->reQuests[i].ttl = 0;
    }
}

/* getLambda()
 * PBP model - returns the probability of
 * burst (POISSON parameter lambda)
 * */
double GenTraffic::getLambda(int SS)
{
    return tfcLoad[SS];
}

/* getPacket()
 * CBR model - returns the number of packet per stream
 * */
int GenTraffic::getPackets(int SS)
{
    return (int)tfcLoad[SS];
}

/* getTTL
 * Randomly generate TTL value between stream MIN and MAX
 * */
int GenTraffic::getTTL()
{
    if (this->infTTL == false)
    {
#ifdef WITH__AKAROA
        return UniformInt(streamTLMin, streamTLMax);
#else
        return intrand(streamTLMax - streamTLMin) + streamTLMin;
#endif
    }
    else
    {
        return INFINITE_STREAM;
    }
}

/* getTrafficType
 * Generate a traffic type (related to post setting of (m,k) deadline
 * No priorities discussed at this implementation
 * */
trafficTypes_t GenTraffic::getTrafficType(void)
{
    bool mk_manual = true;

    // user inputs to use for all streams
    // if wish pre-coded mode the streamK in *.ini to set to 0
    for (int i = 0; i < numSSs; i++)
    {
        if (streamMArray[i] == MAX_STATE ||     // if configured incorrectly
            streamKArray[i] == MAX_STATE)
        {
            mk_manual = false;
            break;
        }
    }

    if (mk_manual)
        return TTYPE_MANUAL;

    if (streamK > 0)
        return TTYPE_INPUT;

    // pre-coded simple type support
    if (!streamRnd)
    {
        return TTYPE_3_4;
    }
    else // Random pre-coded mode
    {
#ifdef WITH__AKAROA
        double r = Uniform(0.0, 4.0);
#else
        double r = uniform(0.0, 4.0, 0);
#endif

        if (r >= 0 && r < 1.0)
            return TTYPE_1_2;
        else if (r >= 1.0 && r < 2.0)
            return TTYPE_2_3;
        else if (r >= 2.0 && r < 3.0)
            return TTYPE_3_4;
        else
            return TTYPE_10_15;
        }
}

/* setMK
 * Sets m and k depending on the available traffic type per request
 * */
void GenTraffic::setMK(int req)
{
    static int m_run[] = {2,4,6,8,10,12,14};    //k=0->6
    static int k_run[] = {4,6,8,10,12,14,16};

    switch (this->reQuests[req].t)
    {
        case TTYPE_1_2:
            this->reQuests[req].m = M_TTYPE_1_2;
            this->reQuests[req].k = K_TTYPE_1_2;
            break;
        default:
        case TTYPE_2_3:
            this->reQuests[req].m = M_TTYPE_2_3;
            this->reQuests[req].k = K_TTYPE_2_3;
            break;
        case TTYPE_3_4:
            this->reQuests[req].m = M_TTYPE_3_4;
            this->reQuests[req].k = K_TTYPE_3_4;
            break;
        case TTYPE_9_11:
            this->reQuests[req].m = M_TTYPE_9_11;
            this->reQuests[req].k = K_TTYPE_9_11;
            break;
        case TTYPE_10_15:
            this->reQuests[req].m = M_TTYPE_10_15;
            this->reQuests[req].k = K_TTYPE_10_15;
            break;
        case TTYPE_13_15:
            this->reQuests[req].m = M_TTYPE_13_15;
            this->reQuests[req].k = K_TTYPE_13_15;
            break;
        case TTYPE_MANUAL:
            this->reQuests[req].m = streamMArray[req];
            this->reQuests[req].k = streamKArray[req];
            break;
        case TTYPE_INPUT:
            if (this->streamM == -1) // a tweak to run as function of (m,k)
            {
                if (this->streamK > 0 && this->streamK < 8)
                {
                    this->reQuests[req].m = m_run[this->streamK-1];
                    this->reQuests[req].k = k_run[this->streamK-1];
                }
                else
                {
                    EV << "Bad (m,k) setting !!!\n";
                    this->reQuests[req].m = M_TTYPE_3_4;
                    this->reQuests[req].k = K_TTYPE_3_4;
                }
            }
            else
            {
                this->reQuests[req].m = this->streamM;
                this->reQuests[req].k = this->streamK;
            }

            break;

    }
}

/* popRequests()
 * Responsible for generation for requests
 * Managing streams, should be called once in a SF
 * */
ssRequest_t* GenTraffic::popRequests(void)
{
    for (int i = 0; i < numSSs; i++)
    {
        // PBP: lambdas are user defined per SS
        if (LOAD_MODEL_PBP == streamType)
        {
            reQuests[i].p = (int)poisson(getLambda(i), 0);
        }
        else // CBR: number of packets per stream is user defined per SS
        {
            reQuests[i].p = getPackets(i);
        }

        // Check if current stream has timed out
        if (INFINITE_STREAM == reQuests[i].ttl)
        {
            reQuests[i].fresh = false;
        }
        else if (reQuests[i].ttl > 0)
        {
            reQuests[i].ttl--;
            reQuests[i].fresh = false;
        }
        else    //establish a new stream
        {
            if (reQuests[i].p > 0)
            {
                reQuests[i].fresh = true;
                // Pick traffic a new random deadline pattern
                reQuests[i].t = getTrafficType();
                // Set a random duration (in super frames)
                reQuests[i].ttl = getTTL();
                // Set m,k to match the selected traffic type
                setMK(i);
                //EV << "Generated RQ(" << i << "): [m=" << reQuests[i].m << ",k=" << reQuests[i].k << "], " << "TTL=" << reQuests[i].ttl << "\n";
            }
            else  //trash current request slot
            {
                reQuests[i].fresh = false;
                reQuests[i].t = TTYPE_ILLEGAL;
                reQuests[i].m = 0;
                reQuests[i].k = 0;
                reQuests[i].ttl = 0;
            }
        }
    }

    return reQuests;
}

}; // namespace
