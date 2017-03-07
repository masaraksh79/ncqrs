#!/bin/sh
res=$1
core=$2
sim=../out/gcc-debug/src/sim.exe
opp=../../../bin/opp_runall
$sim -a
export NEDPATH=/f/Work/Research/omnetpp-4.6/samples/sim/src:/f/Work/Research/omnetpp-4.6/samples/sim/simulations;
#$sim -u Cmdenv -c $res -f omnetpp.ini --debug-on-errors=false -j 6
repmin=0
repmax=$(($3-1))
$opp -j$core -- $sim -r $repmin..$repmax -u Cmdenv -c $res -f omnetpp.ini --debug-on-errors=false

# results
result=`cat results/$res-0.sca |grep iterationvars`
echo ${result[0]}
