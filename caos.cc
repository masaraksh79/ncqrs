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

#include <caos.h>

namespace ncqrs {

caos::caos(int nodes, int tdmaSF, int retxSlots)
{
    this->numSSs = nodes;
    this->tdmaSF = tdmaSF;
    this->retxSlots = retxSlots;
    this->optNs = new int[numSSs];

    for (int i = 0; i < numSSs; i++)
        optNs[i] = 0;
}

caos::~caos() {}

/* Solver of optimal problem max(lnF)
 * Discretization of double coefficients to integers */
void caos::findOptimalNs(int* corrupt_stream, double* estErrors)
{
    int nCorrupt = 0;
    int i, steps = 0;
    double err;
    double HS_GAP = 0.01;
    double lambda = 1;

    // skip while estErrors are ZERO
    for (i = 0; i < numSSs; i++)
    {
        nCorrupt += corrupt_stream[i];
        if (estErrors[i] == 0)
            return;
    }

    if (!nCorrupt)   // no corrupt streams
    {
        EV << "Skipping CAOS: no corrupt streams.\n";
        for (i = 0; i < numSSs; i++)
            optNs[i] = 0;
        return;
    }
    else
    {
        lambda = this->expsearch(&steps, HS_GAP, &err, corrupt_stream, estErrors);
        EV << "Found lambda by EXPSearch = "<< lambda << " within " << steps <<" steps, error=" << err <<"\n";
    }

    if (lambda < 0)
    {
        double n[numSSs];
        double n_check = 0.0;   // to see n's sum is equal to N
        int ni_check = 0;
        int left_ts = 0;

        EV << "***********************\n";

        for (i = 0; i < numSSs; i++)
        {
            if (corrupt_stream[i])
                n[i] = Coeff_N(lambda, i, estErrors);
            else
                n[i] = 0;

            n_check += n[i];
        }

        EV << "Sum of n's:   " << n_check << "\n---------------------------------------------------\n";

        for (i = 0; i < numSSs; i++)
            optNs[i] = 0;

        for (i = 0; i < numSSs; i++)
        {
            if (corrupt_stream[i])
            {
                optNs[i] = lround(n[i]);
                ni_check += optNs[i];
                EV << "n"<< i << ":"<< n[i] << "  rounded " << lround(n[i]) << "\n";
            }
        }
        EV << "---------------------------------------------------\n";

        // additional amendment for under usage of retxSlots
        // works for streams that received 0<n<0.5 -> 0
        // will not allocate the left-over to streams with n >=1
        left_ts = retxSlots - ni_check;

        if (left_ts <= 0)
            return;

        for (i = 0; i < numSSs; i++)
        {
            if ( corrupt_stream[i] && (n[i] > optNs[i]) )
            {
                optNs[i]++;
                left_ts--;
                if (left_ts <=0)
                    break;
            }
        }

        EV << "adjusted 0 n's:\n---------------------------------------------------\n";
        for (i = 0; i < numSSs; i++)
        {
            if (corrupt_stream[i])
                EV << "n"<< i << " " << n[i] << " ->  allocate ->  " << optNs[i] << "\n";
        }
    }
}

double caos::expsearch(int* steps, double gap, double* err, int* corrupt_stream, double* estErrors)
{
    double m = -0.1;
    double s, lhs;

    lhs = this->LHS(m, corrupt_stream, estErrors);

    if (lhs < retxSlots)
        s = binarysearch(steps, m, 0, corrupt_stream, gap, err, estErrors);
    else
    {
        while (lhs > retxSlots)
        {
            m *= 2;
            lhs = this->LHS(m, corrupt_stream, estErrors);
        }

        s = binarysearch(steps, m, 0, corrupt_stream, gap, err, estErrors);
    }

    return s;
}

double caos::binarysearch(int* steps, double m, double n, int* corrupt_stream, double agap, double *err, double* estErrors)
{
    double right = m, left = n, middle;
    double lhs = 0;
    double rhs = retxSlots;
    int tmp_step = 0;
    double gap = agap;

    while (fabs(lhs - rhs) > gap)
    {
        (*steps)++;
        tmp_step++;

        middle = (right + left) / 2;

        lhs = this->LHS(middle, corrupt_stream, estErrors);

        if (fabs(lhs - rhs) <= gap)
        {
            break;
        }
        else if (lhs < rhs)
        {
            right = middle;
        }
        else    // rhs > lhs
        {
            left = middle;
        }
#if 1
        if (tmp_step > 100)
        {
            tmp_step = 0;
            gap *= 2;
            break;
        }
#endif
    }

    *err = fabs(lhs - rhs);
    return middle;
}

/* LHS is
 * ln(x/x+lnp_1)^psi_1)(ln(x/x+lnp_2)^psi_2)*...*(ln(x/x+lnp_k)^psi_k
 * */
double caos::LHS(double x, int* corrupt_stream, double* estErrors)
{
    double b[numSSs];
    double res = 0.0;
    int i;

    for (i = 0; i < numSSs; i++)
    {
        if (!corrupt_stream[i])
            continue;

        b[i] = x / (x + log(estErrors[i]));
        res += (1.0 / log(estErrors[i])) * log(b[i]);
    }

    return res;
}

/* Find optimal n's from a solved lambda (x) */
double caos::Coeff_N(double x, int idx, double* estErrors)
{
    return log(x/(x+log(estErrors[idx])))/log(estErrors[idx]);
}

int caos::getOptNs(int i)
{
    return optNs[i];
}


} /* namespace ncqrs */
