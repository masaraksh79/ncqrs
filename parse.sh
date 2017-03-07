#!/bin/bash

res=$1

if [ "x${res}" == "x-h" ]; then
	echo "==========================================================================" 
	echo "= Parsing out violation Rate of multiple scenario files                  =" 				  
	echo "= Specify [arg1=ScenarioName] [arg2=repetitions] [arg3=sample_size]      ="
	echo "= or Specify [arg1=ScenarioName] [arg2=show] to print the output simfile ="
    echo "= Saves data to scenario.sim file under current dir                      ="	 
	echo "==========================================================================" 
	exit
fi 

iter=$2
sample=$3

if [ "x${res}" == "xremove" ]; then
   scen=$iter
   start=$sample
   end=$4
   while [ $start -le $end ]; do
      rm results/${scen}-${start}.sca
      start=$((start+1))
   done
   
   exit
fi
convtests=$4	# expect 1 to run averaging on convP and not on vioRate or nothing

# results
num=$((iter-1))
fname=${res}.sim
if [ "x${iter}" == "xshow" ]; then
	if [ -f ${fname} ]; then
		cat ${fname}
	else
		echo "Simulation file ${res}.sim doesn't exist"
	fi
	exit
fi

#hdr=`cat results/${res}-0.sca |grep run`
#echo ${hdr[0]} > ${fname}
touch ${fname}
fnameerr="${fname}.err"

i=0
j=1
a=0
vioavg=0
echo -n "Processing"
while [ $i -le $num ]; do
  conv=`cat results/${res}-${i}.sca |grep convP`
  result=`cat results/${res}-${i}.sca |grep vioAvgRate`
  caosFail=`cat results/${res}-${i}.sca |grep caosFailures`
  avgAction=`cat results/${res}-${i}.sca |grep sAvgAction`
  
  if ! [ "x${caosFail}" == "x" ]; then
  	caoscnt=`echo ${caosFail[0]} |cut -d' ' -f4`
  	echo "CaosFailures=${caoscnt}" >> ${fnameerr}
  fi
  
  if [ "x${convtests}" == "x1" ]; then
  	inp=$conv
  elif [ "x${convtests}" == "x2" ]; then
  	inp=$avgAction
  else
  	inp=$result
  fi
  
  viorate=`echo ${inp[0]} |cut -d' ' -f4`
  viostr="${viostr} ${viorate}"
  if [ $j -eq $sample ];then #start new averaging
    vioavg=`echo ${viostr} | awk -v CONVFMT=%.17g 'BEGIN{sum=0;count=1} {for(count;count<=NF;count++){ sum = sum+$count;}} END{ print sum/NF;}'`
    echo ${vioavg} >> ${fname}
    viostr=
    vioavg=0
   j=0
  fi
  j=$((j+1))
 
  # normal sample count   
  if [ $((i % 10)) -eq 0 ];then
     echo -n "."
  fi
  
  i=$((i+1))
done
echo ""
echo "Copying to shared folder"
cp ${fname} ../../../../Downloads/
