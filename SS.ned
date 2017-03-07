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

//
// Immediately sends out any message it receives. It can optionally generate
// a message at the beginning of the simulation, to bootstrap the process.
//
simple SS
{
    parameters:
        int offsetID;
        int tdmaTs;
        int numSSs;						// Used only for grabbing pErr which are SS_ID oriented
		string pPER;					// real link value error prob
		double pPERch;					// by how much to change the, negative reduces
		int pPERtime;					// sim time to change the PER at
		int chanType;					// channel model in use (0 - static, 1 - 2state markov)
		double EH1;						// sojorn Bad-State time, in packets e.g. 5,10,...
		double eB;						// error chance in Bad-State probability
		double B;						// Bustiness index E[h1]/E[h0]
    gates:
        input in;
        output out;
}
