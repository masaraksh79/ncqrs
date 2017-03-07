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

#ifndef PKTCONSTRUCT_H_
#define PKTCONSTRUCT_H_

#include <omnetpp.h>
#include "GenTraffic.h"
#include "pktAlloc_m.h"

namespace ncqrs {

typedef struct
{
    msgTypes_t ptype;       /* packet type (T, R or E) */
    trafficTypes_t ttype;   /* type of traffic corresponding to (m,k) */
    int ss_id;              /* allocated ID */
    int pkts;               /* allocated # packets */
}
ssAlloc_t;

class pktConstruct
{
    private:
        int offsetID;
        int maxChunks;
    public:
        pktConstruct(int nodes, int maxChunks);
        ~pktConstruct();

        void init(PktAlloc* pkt);
        int add(PktAlloc* pkt, int chunk, msgTypes_t ptype, trafficTypes_t ttype, int ssID, int pkts);
        void report(PktAlloc* pkt);
};

} /* namespace rlsch */

#endif /* SS_H_ */
