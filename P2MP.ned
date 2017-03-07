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
// Main network (Point [BS] to Multipoint [SSs])
//
network P2MP
{
    parameters:
        int offsetID = default(3);	// Offset of SS IDs in the system (to get 0,1,2,...,N)
        int numSSs; 				// Number of Remote Stations 
        int tdmaTs;					// System timeslot size
        int tdmaSF;					// Superframe size (in timeslots)
        string pPER;				// Link error probabilities for static model (uniform<> generated)
        int chanType = default(0);  // channel model in use (0 - static, 1 - 2state markov)
    submodules:
        bs: BS { 
            @display("i=device/router_l"); 
            tdmaTs 		= tdmaTs;
            tdmaSF 		= tdmaSF;
            numSSs 		= numSSs;
            pPER 		= pPER;
            chanType	= chanType;
            offsetID 	= offsetID;
            };
        ss[numSSs]: SS { 
            @display("i=device/terminal");
            tdmaTs 		= tdmaTs;
            numSSs 		= numSSs;
			pPER 		= pPER;
			chanType	= chanType;
			offsetID 	= offsetID;
            };
    connections:
    for i=0..numSSs-1 { 
        bs.out[i] --> { delay=3us; } --> ss[i].in;
        bs.in[i] <-- { delay=50us; } <-- ss[i].out;
    }
}

