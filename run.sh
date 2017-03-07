#!/bin/bash

# Args
# app [scenario_name] [cpus] [repetitions] [samplesize] [inifile] [conv]
res=$1
if [ x"$res" == "x-h" ]; then
	echo "app [scenario_name] [cpus] [repetitions] [samplesize] [inifile] [conv]"
	exit
fi

core=$2
sim=./out/gcc-debug/ncqrs
opp=./../../bin/opp_runall
$sim -a
export NEDPATH=/home/cosc/student/yma61/omnetpp-4.5/samples/ncqrs;
repmin=0
repmax=$(($3-1))

if [ "x$5" == "x" ];then
	ini=omnetpp.ini
else
	ini=$5
fi 

$opp -j$core -- $sim -r $repmin..$repmax -u Cmdenv -c $res -f $ini --debug-on-errors=false

# parse the results using sample size arg
if [ "x${6}" == "xconv" ]; then
	./parse.sh $res $3 $4 1
elif [ "x${6}" == "xaction" ]; then
	./parse.sh $res $3 $4 2
else
	./parse.sh $res $3 $4
fi
# remove result files
./parse.sh remove $res $repmin $repmax
