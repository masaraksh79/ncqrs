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
simple BS
{
    parameters:
        int offsetID;
        int numSSs;
        int tdmaTs;
        int tdmaSF;
        int latencySFs;					// number of SFs can fix back for (m,k)-firm buffer policies
        int maxChunks;					// maximum of allowed space in ALLOC packet in total (assume 1pkt/chunk)
        int retxChunks;					// maximum of retransmit alloc allowed slots in ALLOC (any num# packets/chunk)
        int retxSlots;					// maximum of retransmit packets 
        int schedMethod;				// selection of a scheduler technique to have in sim
        								// existing are RL(0,1,2) DBP stands for 3
        								// and also 4 which is Optimal Coefficient Allocation
        								// and 5 is DBRR
        double pErrInitial;				// dummy pErr value for scheduler's knowledge
        double pErrEWMA;				// Alfa factor for Exponentially weighted MA
        double rewardReduce;			// Rewards for changes in a stream distance from failure
        double rewardViolate;
        double rewardSlots;				// How much penalty the algorithm gets for allocating more than needed
        int actPrefNodeID;				// node to look at with respect to RL action preferences
        string pPER;					// real link value error prob
        int chanType;					// channel model in use (0 - static, 1 - 2state markov)
        int	streamType;					// stream Type (0 - Periodic const, 1 - Poisson burst) 
        int	streamTLMin = default(10);	// stream Duration (in super frames) Min - Max
        int streamTLMax = default(50);
		int streamM = default(0);		// the m out of (m,k)-firm, internal default will be used if not set
		int streamK = default(0);		// the k out of (m,k)-firm, internal default will be used if not set  
		string streamMArr;				// example "3 4 5 7"
		string streamKArr;  			// example "4 6 7 10"
		bool streamRnd = default(false);// If set to true -> enables random selection for stream (mi,ki) from existing m,k-list     								
        bool infTTL = default(true);	// flag to enable an infinite stream duration 				
        string tfcLoad;					// For stream Type 0  - number of packets per SF per SS
        								// For 1 - SSs poisson probability lambdas for traffic gen
        								
		double rl_ac_alfa;				// RL alfa coefficient
		double rl_ac_beta;				// RL beta coefficient
		double rl_ac_gamma;				// RL gamma coefficient
		
		double rl_q_alfa; 				// RL alfa coefficient (Q-Learning)
		double rl_q_gamma;				// RL gamma coefficient (Q-Learning)
		
		double trimRank;				// Trim rank
		
		double maVioFactor;				// Moving average history factor for evaluation V(t)
		
		int conv_startx;				// number of TSs where we start waiting for the cross-over point
		double conv_upfac;				// limit to be inside conv
		int conv_batch;					// batch size for mean
		int slots_per_usr;				// max # retx slots per user
        								
        @signal[numActPrefs*](type="double");
        @statisticRL[actPrefs](record=vector(mean);title="preferred action for state $");
        @signal[numBadRx*](type="long");
        @statisticSS[rxBadPkts](record=vector(sum);title="bad frames from $");
        @signal[vioRate*](type="double");
        @statisticSS[vioRates](record=vector(mean);title="Violation ratio for node $");
        @signal[rlAvgValue*](type="double");
        @statisticSS[rlAvgValueFunction](record=vector(mean);title="Average reward function $");
        @signal[sAvgAction](type="double");
        @statistic[sAvgAction](record=vector(mean);title="Average stream requested retransmissions per SF");
        @signal[vioAvgRate](type="double");
        @statistic[vioAvgRate](record=vector(mean);title="Average network violation rate");
        @signal[pErrAvg](type="double");
        @statistic[pErrAvg](record=vector(mean);title="Average net packet error rate");
            gates:
        	input in[numSSs];
        	output out[numSSs];
}
