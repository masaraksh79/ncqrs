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

#ifndef CAOS_H_
#define CAOS_H_

#include <omnetpp.h>

namespace ncqrs {

class caos {
private:
    int numSSs;
    int tdmaSF;
    int retxSlots;
    int* optNs;
public:
    caos(int nodes, int tdmaSF, int retxSlots);
    virtual ~caos();
    void    findOptimalNs(int* corrupt_stream, double* estErrors);
    double  LHS(double x, int*, double* estErrors);
    double  binarysearch(int* steps, double m, double n, int* corrupt_stream, double gap, double* err, double* estErrors);
    double  expsearch(int* steps, double gap, double* err, int* corrupt_stream, double* estErrors);
    double  Coeff_N(double x, int idx, double* estErrors);
    int     getOptNs(int i);
};

} /* namespace ncqrs */

#endif /* CAOS_H_ */
