#!/bin/bash

cpus=$1

alphagamma=0
static=1
static2=0
markov=0
markov2=0
large=0

repet=142500
sampl=150

if [ $alphagamma -eq 1 ]; then
# static channels opt
./run.sh FindOptAlphaGamma9 $cpus $repet $sampl alphagamma.ini
./run.sh FindOptAlphaGamma10 $cpus $repet $sampl alphagamma.ini
fi

repet=400
sampl=100

if [ $static -eq 1 ]; then

# static.ini (3,4)-firm 1u-2rtx

# homogeneous scenarios for 5U (1000 runs total, 20 values)
./run.sh PerfHo5UPer01 $cpus $repet $sampl static.ini
./run.sh PerfHo5UPer02 $cpus $repet $sampl static.ini
./run.sh PerfHo5UPer03 $cpus $repet $sampl static.ini
./run.sh PerfHo5UPer04 $cpus $repet $sampl static.ini
./run.sh PerfHo5UPer05 $cpus $repet $sampl static.ini

# heterogeneous scenarios for 5U (1000 runs total, 20 values)
./run.sh PerfHe5UPer01 $cpus $repet $sampl static.ini
./run.sh PerfHe5UPer02 $cpus $repet $sampl static.ini
./run.sh PerfHe5UPer03 $cpus $repet $sampl static.ini
./run.sh PerfHe5UPer04 $cpus $repet $sampl static.ini
./run.sh PerfHe5UPer05 $cpus $repet $sampl static.ini

# homogeneous scenarios for 20U (1000 runs total, 20 values)
./run.sh PerfHo20UPer01 $cpus $repet $sampl static.ini
./run.sh PerfHo20UPer015 $cpus $repet $sampl static.ini
./run.sh PerfHo20UPer02 $cpus $repet $sampl static.ini
./run.sh PerfHo20UPer025 $cpus $repet $sampl static.ini
./run.sh PerfHo20UPer03 $cpus $repet $sampl static.ini

# heterogeneous scenarios for 20U (1000 runs total, 20 values)
./run.sh PerfHe20UPer01 $cpus $repet $sampl static.ini
./run.sh PerfHe20UPer015 $cpus $repet $sampl static.ini
./run.sh PerfHe20UPer02 $cpus $repet $sampl static.ini
./run.sh PerfHe20UPer025 $cpus $repet $sampl static.ini
./run.sh PerfHe20UPer03 $cpus $repet $sampl static.ini

# static.ini (9,10)-firm 1u-2rtx

# homogeneous scenarios for 5U (1000 runs total, 20 values)
./run.sh Perf3Ho5UPer01 $cpus $repet $sampl static3.ini
./run.sh Perf3Ho5UPer02 $cpus $repet $sampl static3.ini
./run.sh Perf3Ho5UPer03 $cpus $repet $sampl static3.ini
./run.sh Perf3Ho5UPer04 $cpus $repet $sampl static3.ini
./run.sh Perf3Ho5UPer05 $cpus $repet $sampl static3.ini

# heterogeneous scenarios for 5U (1000 runs total, 20 values)
./run.sh Perf3He5UPer01 $cpus $repet $sampl static3.ini
./run.sh Perf3He5UPer02 $cpus $repet $sampl static3.ini
./run.sh Perf3He5UPer03 $cpus $repet $sampl static3.ini
./run.sh Perf3He5UPer04 $cpus $repet $sampl static3.ini
./run.sh Perf3He5UPer05 $cpus $repet $sampl static3.ini

# homogeneous scenarios for 20U (1000 runs total, 20 values)
./run.sh Perf3Ho20UPer01 $cpus $repet $sampl static3.ini
./run.sh Perf3Ho20UPer015 $cpus $repet $sampl static3.ini
./run.sh Perf3Ho20UPer02 $cpus $repet $sampl static3.ini
./run.sh Perf3Ho20UPer025 $cpus $repet $sampl static3.ini
./run.sh Perf3Ho20UPer03 $cpus $repet $sampl static3.ini

# heterogeneous scenarios for 20U (1000 runs total, 20 values)
./run.sh Perf3He20UPer01 $cpus $repet $sampl static3.ini
./run.sh Perf3He20UPer015 $cpus $repet $sampl static3.ini
./run.sh Perf3He20UPer02 $cpus $repet $sampl static3.ini
./run.sh Perf3He20UPer025 $cpus $repet $sampl static3.ini
./run.sh Perf3He20UPer03 $cpus $repet $sampl static3.ini

fi

if [ $static2 -eq 1 ]; then

# static2.ini (14,15)-firm 1u-1rtx

# homogeneous scenarios for 5U (1000 runs total, 20 values)
./run.sh Perf2Ho5UPer01 $cpus $repet $sampl static2.ini
./run.sh Perf2Ho5UPer015 $cpus $repet $sampl static2.ini
./run.sh Perf2Ho5UPer02 $cpus $repet $sampl static2.ini
./run.sh Perf2Ho5UPer025 $cpus $repet $sampl static2.ini
./run.sh Perf2Ho5UPer03 $cpus $repet $sampl static2.ini

# heterogeneous scenarios for 5U (1000 runs total, 20 values)
./run.sh Perf2He5UPer01 $cpus $repet $sampl static2.ini
./run.sh Perf2He5UPer015 $cpus $repet $sampl static2.ini
./run.sh Perf2He5UPer02 $cpus $repet $sampl static2.ini
./run.sh Perf2He5UPer025 $cpus $repet $sampl static2.ini
./run.sh Perf2He5UPer03 $cpus $repet $sampl static2.ini

# homogeneous scenarios for 10U (1000 runs total, 20 values)
./run.sh Perf2Ho10UPer01 $cpus $repet $sampl static2.ini
./run.sh Perf2Ho10UPer015 $cpus $repet $sampl static2.ini
./run.sh Perf2Ho10UPer02 $cpus $repet $sampl static2.ini
./run.sh Perf2Ho10UPer025 $cpus $repet $sampl static2.ini
./run.sh Perf2Ho10UPer03 $cpus $repet $sampl static2.ini

# heterogeneous scenarios for 10U (1000 runs total, 20 values)
./run.sh Perf2He10UPer01 $cpus $repet $sampl static2.ini
./run.sh Perf2He10UPer015 $cpus $repet $sampl static2.ini
./run.sh Perf2He10UPer02 $cpus $repet $sampl static2.ini
./run.sh Perf2He10UPer025 $cpus $repet $sampl static2.ini
./run.sh Perf2He10UPer03 $cpus $repet $sampl static2.ini

fi

if [ $markov -eq 1 ]; then

repet=16800
sampl=150

# ======================================================================
# We repeat for 5, 10 and 15 nodes, homogeneous PER of 0.2
# Running on 4 values of B and 7 values of E[H1] we try 16 combinations
# of these (all) on each one of the scheduling policies
# ======================================================================

# homogeneous scenarios for 5U (4480 runs total, 64 values)
./run.sh PerfHoU5Per02Markov $cpus $repet $sampl markov.ini

# homogeneous scenarios for 10U (4480 runs total, 64 values)
./run.sh PerfHoU10Per02Markov $cpus $repet $sampl markov.ini

# homogeneous scenarios for 15U (4480 runs total, 64 values)
./run.sh PerfHoU15Per02Markov $cpus $repet $sampl markov.ini

repet=4800
sampl=150

# ====================================================================
# Each one of next 10 streams gets same bad holding time E[H1]
# B=0.7, we repeat after growing the E[H1] for all links the same
# PER is different between links as well, three exp. here: 0.1,0.2,0.3
# ====================================================================

# heterogeneous scenarios for 10U (1120 runs total, 16 values)
./run.sh PerfHeU10Per01MarkovB07 $cpus $repet $sampl markov.ini

# heterogeneous scenarios for 10U (1120 runs total, 16 values)
./run.sh PerfHeU10Per02MarkovB07 $cpus $repet $sampl markov.ini

# heterogeneous scenarios for 10U (1120 runs total, 16 values)
./run.sh PerfHeU10Per03MarkovB07 $cpus $repet $sampl markov.ini

repet=600
sampl=150

# ============================================================================================
# Each one of next 10 streams gets different bad holding time E[H1] from 5 - 50
# every scenario introduces different PER between links as well, changing avg PER: 0.1-0.3
# ============================================================================================

# EH1&PER heterogeneous scenarios for 10U avgPER=0.1 (320 runs total, 4 values)
./run.sh PerfHeU10Per01MarkovB05VaryEH1 $cpus $repet $sampl markov.ini

# EH1&PER heterogeneous scenarios for 10U avgPER=0.15 (320 runs total, 4 values)
./run.sh PerfHeU10Per015MarkovB05VaryEH1 $cpus $repet $sampl markov.ini

# EH1&PER heterogeneous scenarios for 10U avgPER=0.2 (320 runs total, 4 values)
./run.sh PerfHeU10Per02MarkovB05VaryEH1 $cpus $repet $sampl markov.ini

# EH1&PER heterogeneous scenarios for 10U avgPER=0.25 (320 runs total, 4 values)
./run.sh PerfHeU10Per025MarkovB05VaryEH1 $cpus $repet $sampl markov.ini

# EH1&PER heterogeneous scenarios for 10U avgPER=0.3 (320 runs total, 4 values)
./run.sh PerfHeU10Per03MarkovB05VaryEH1 $cpus $repet $sampl markov.ini

# ============================================================================================
# Each one of next 20 streams gets different bad holding time E[H1] from 5 - 100
# every scenario has same PER set for all nodes (setting similar eB), 5 exp. @PER: 0.1-0.3
# ============================================================================================

# EH1-heterogeneous scenarios for 20U Hom-PER=0.1 (320 runs total, 4 values)
./run.sh PerfHeU20Per01MarkovB05VaryEH1 $cpus $repet $sampl markov.ini

# EH1-heterogeneous scenarios for 20U Hom-PER=0.15 (320 runs total, 4 values)
./run.sh PerfHeU20Per015MarkovB05VaryEH1 $cpus $repet $sampl markov.ini

# EH1-heterogeneous scenarios for 20U Hom-PER=0.2 (320 runs total, 4 values)
./run.sh PerfHeU20Per02MarkovB05VaryEH1 $cpus $repet $sampl markov.ini

# EH1-heterogeneous scenarios for 20U Hom-PER=0.25 (320 runs total, 4 values)
./run.sh PerfHeU20Per025MarkovB05VaryEH1 $cpus $repet $sampl markov.ini

# EH1-heterogeneous scenarios for 20U Hom-PER=0.3 (320 runs total, 4 values)
./run.sh PerfHeU20Per03MarkovB05VaryEH1 $cpus $repet $sampl markov.ini

# ============================================================================================
# Each one of next 15 streams gets different bad holding time E[H1], different B and PER
# The 5 scenarios rated by average PER as 0.1:0.05:3
# ============================================================================================

# Fully heterogeneous scenarios for 15U Avg-PER=0.1 (320 runs total, 4 values)
./run.sh PerfHeU15MarkovVaryAllP010 $cpus $repet $sampl markov.ini

# Fully heterogeneous scenarios for 15U Avg-PER=0.15 (320 runs total, 4 values)
./run.sh PerfHeU15MarkovVaryAllP015 $cpus $repet $sampl markov.ini

# Fully heterogeneous scenarios for 15U Avg-PER=0.2 (320 runs total, 4 values)
./run.sh PerfHeU15MarkovVaryAllP020 $cpus $repet $sampl markov.ini

# Fully heterogeneous scenarios for 15U Avg-PER=0.25 (320 runs total, 4 values)
./run.sh PerfHeU15MarkovVaryAllP025 $cpus $repet $sampl markov.ini

# Fully heterogeneous scenarios for 15U Avg-PER=0.3 (320 runs total, 4 values)
./run.sh PerfHeU15MarkovVaryAllP030 $cpus $repet $sampl markov.ini

fi

if [ $markov2 -eq 1 ]; then

repet=600
sampl=150

# ============================================================================================
# Each one of next 5 streams gets different bad holding time E[H1], different B and PER
# The 5 scenarios rated by average PER as 0.1:0.05:3
# ============================================================================================

# Fully heterogeneous scenarios for 5U Avg-PER=0.1 (320 runs total, 4 values)
./run.sh Perf2HeU5MarkovVaryAllP010 $cpus $repet $sampl markov2.ini

# Fully heterogeneous scenarios for 5U Avg-PER=0.15 (320 runs total, 4 values)
./run.sh Perf2HeU5MarkovVaryAllP015 $cpus $repet $sampl markov2.ini

# Fully heterogeneous scenarios for 5U Avg-PER=0.2 (320 runs total, 4 values)
./run.sh Perf2HeU5MarkovVaryAllP020 $cpus $repet $sampl markov2.ini

# Fully heterogeneous scenarios for 5U Avg-PER=0.25 (320 runs total, 4 values)
./run.sh Perf2HeU5MarkovVaryAllP025 $cpus $repet $sampl markov2.ini

# Fully heterogeneous scenarios for 5U Avg-PER=0.3 (320 runs total, 4 values)
./run.sh Perf2HeU5MarkovVaryAllP030 $cpus $repet $sampl markov2.ini

repet=16800
sampl=150

# homogeneous scenarios for 5U (4480 runs total, 64 values)
./run.sh Perf2HoU5Per02Markov $cpus $repet $sampl markov2.ini

fi

if [ $large -eq 1 ]; then

repet=400
sampl=100

# ============================================================================================
# Static homogeneous scenario Per=0.1 for 10,20,...,100 units
# ============================================================================================

# Static homogeneous scenario Per=0.1 for 10 units (200 runs total, 4 values)
./run.sh LargeNet10UPer01 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.1 for 20 units (200 runs total, 4 values)
./run.sh LargeNet20UPer01 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.1 for 30 units (200 runs total, 4 values)
./run.sh LargeNet30UPer01 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.1 for 40 units (200 runs total, 4 values)
./run.sh LargeNet40UPer01 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.1 for 50 units (200 runs total, 4 values)
./run.sh LargeNet50UPer01 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.1 for 60 units (200 runs total, 4 values)
./run.sh LargeNet60UPer01 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.1 for 70 units (200 runs total, 4 values)
./run.sh LargeNet70UPer01 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.1 for 80 units (200 runs total, 4 values)
./run.sh LargeNet80UPer01 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.1 for 90 units (200 runs total, 4 values)
./run.sh LargeNet90UPer01 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.1 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNet100UPer01 $cpus $repet $sampl large.ini

# ============================================================================================
# Static homogeneous scenario Per=0.15 for 10,20,...,100 units
# ============================================================================================

# Static homogeneous scenario Per=0.15 for 10 units (200 runs total, 4 values)
./run.sh LargeNet10UPer015 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.15 for 20 units (200 runs total, 4 values)
./run.sh LargeNet20UPer015 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.15 for 30 units (200 runs total, 4 values)
./run.sh LargeNet30UPer015 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.15 for 40 units (200 runs total, 4 values)
./run.sh LargeNet40UPer015 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.15 for 50 units (200 runs total, 4 values)
./run.sh LargeNet50UPer015 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.15 for 60 units (200 runs total, 4 values)
./run.sh LargeNet60UPer015 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.15 for 70 units (200 runs total, 4 values)
./run.sh LargeNet70UPer015 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.15 for 80 units (200 runs total, 4 values)
./run.sh LargeNet80UPer015 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.15 for 90 units (200 runs total, 4 values)
./run.sh LargeNet90UPer015 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.15 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNet100UPer015 $cpus $repet $sampl large.ini

# ============================================================================================
# Static homogeneous scenario Per=0.2 for 10,20,...,100 units
# ============================================================================================

# Static homogeneous scenario Per=0.2 for 10 units (200 runs total, 4 values)
./run.sh LargeNet10UPer02 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.2 for 20 units (200 runs total, 4 values)
./run.sh LargeNet20UPer02 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.2 for 30 units (200 runs total, 4 values)
./run.sh LargeNet30UPer02 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.2 for 40 units (200 runs total, 4 values)
./run.sh LargeNet40UPer02 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.2 for 50 units (200 runs total, 4 values)
./run.sh LargeNet50UPer02 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.2 for 60 units (200 runs total, 4 values)
./run.sh LargeNet60UPer02 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.2 for 70 units (200 runs total, 4 values)
./run.sh LargeNet70UPer02 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.2 for 80 units (200 runs total, 4 values)
./run.sh LargeNet80UPer02 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.2 for 90 units (200 runs total, 4 values)
./run.sh LargeNet90UPer02 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.2 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNet100UPer02 $cpus $repet $sampl large.ini

# ============================================================================================
# Static homogeneous scenario Per=0.25 for 10,20,...,100 units
# ============================================================================================

# Static homogeneous scenario Per=0.25 for 10 units (200 runs total, 4 values)
./run.sh LargeNet10UPer025 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.25 for 20 units (200 runs total, 4 values)
./run.sh LargeNet20UPer025 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.25 for 30 units (200 runs total, 4 values)
./run.sh LargeNet30UPer025 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.25 for 40 units (200 runs total, 4 values)
./run.sh LargeNet40UPer025 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.25 for 50 units (200 runs total, 4 values)
./run.sh LargeNet50UPer025 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.25 for 60 units (200 runs total, 4 values)
./run.sh LargeNet60UPer025 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.25 for 70 units (200 runs total, 4 values)
./run.sh LargeNet70UPer025 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.25 for 80 units (200 runs total, 4 values)
./run.sh LargeNet80UPer025 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.25 for 90 units (200 runs total, 4 values)
./run.sh LargeNet90UPer025 $cpus $repet $sampl large.ini

# Static homogeneous scenario Per=0.25 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNet100UPer025 $cpus $repet $sampl large.ini

# ============================================================================================
# GE channel (B=0.4 E[h1]=5) scenario Per=0.1 for 10,20,...,100 units
# ============================================================================================

# GE channel scenario Per=0.1 for 10 units (200 runs total, 4 values)
./run.sh LargeNetG1E10UPer01 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.1 for 20 units (200 runs total, 4 values)
./run.sh LargeNetG1E20UPer01 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.1 for 30 units (200 runs total, 4 values)
./run.sh LargeNetG1E30UPer01 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.1 for 40 units (200 runs total, 4 values)
./run.sh LargeNetG1E40UPer01 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.1 for 50 units (200 runs total, 4 values)
./run.sh LargeNetG1E50UPer01 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.1 for 60 units (200 runs total, 4 values)
./run.sh LargeNetG1E60UPer01 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.1 for 70 units (200 runs total, 4 values)
./run.sh LargeNetG1E70UPer01 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.1 for 80 units (200 runs total, 4 values)
./run.sh LargeNetG1E80UPer01 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.1 for 90 units (200 runs total, 4 values)
./run.sh LargeNetG1E90UPer01 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.1 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNetG1E100UPer01 $cpus $repet $sampl largeGE1.ini

# ============================================================================================
# GE channel scenario Per=0.15 for 10,20,...,100 units
# ============================================================================================

# GE channel scenario Per=0.15 for 10 units (200 runs total, 4 values)
./run.sh LargeNetG1E10UPer015 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.15 for 20 units (200 runs total, 4 values)
./run.sh LargeNetG1E20UPer015 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.15 for 30 units (200 runs total, 4 values)
./run.sh LargeNetG1E30UPer015 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.15 for 40 units (200 runs total, 4 values)
./run.sh LargeNetG1E40UPer015 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.15 for 50 units (200 runs total, 4 values)
./run.sh LargeNetG1E50UPer015 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.15 for 60 units (200 runs total, 4 values)
./run.sh LargeNetG1E60UPer015 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.15 for 70 units (200 runs total, 4 values)
./run.sh LargeNetG1E70UPer015 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.15 for 80 units (200 runs total, 4 values)
./run.sh LargeNetG1E80UPer015 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.15 for 90 units (200 runs total, 4 values)
./run.sh LargeNetG1E90UPer015 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.15 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNetG1E100UPer015 $cpus $repet $sampl largeGE1.ini

# ============================================================================================
# GE channel scenario Per=0.2 for 10,20,...,100 units
# ============================================================================================

# GE channel scenario Per=0.2 for 10 units (200 runs total, 4 values)
./run.sh LargeNetG1E10UPer02 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.2 for 20 units (200 runs total, 4 values)
./run.sh LargeNetG1E20UPer02 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.2 for 30 units (200 runs total, 4 values)
./run.sh LargeNetG1E30UPer02 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.2 for 40 units (200 runs total, 4 values)
./run.sh LargeNetG1E40UPer02 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.2 for 50 units (200 runs total, 4 values)
./run.sh LargeNetG1E50UPer02 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.2 for 60 units (200 runs total, 4 values)
./run.sh LargeNetG1E60UPer02 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.2 for 70 units (200 runs total, 4 values)
./run.sh LargeNetG1E70UPer02 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.2 for 80 units (200 runs total, 4 values)
./run.sh LargeNetG1E80UPer02 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.2 for 90 units (200 runs total, 4 values)
./run.sh LargeNetG1E90UPer02 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.2 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNetG1E100UPer02 $cpus $repet $sampl largeGE1.ini

# ============================================================================================
# GE channel scenario Per=0.25 for 10,20,...,100 units
# ============================================================================================

# GE channel scenario Per=0.25 for 10 units (200 runs total, 4 values)
./run.sh LargeNetG1E10UPer025 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.25 for 20 units (200 runs total, 4 values)
./run.sh LargeNetG1E20UPer025 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.25 for 30 units (200 runs total, 4 values)
./run.sh LargeNetG1E30UPer025 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.25 for 40 units (200 runs total, 4 values)
./run.sh LargeNetG1E40UPer025 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.25 for 50 units (200 runs total, 4 values)
./run.sh LargeNetG1E50UPer025 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.25 for 60 units (200 runs total, 4 values)
./run.sh LargeNetG1E60UPer025 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.25 for 70 units (200 runs total, 4 values)
./run.sh LargeNetG1E70UPer025 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.25 for 80 units (200 runs total, 4 values)
./run.sh LargeNetG1E80UPer025 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.25 for 90 units (200 runs total, 4 values)
./run.sh LargeNetG1E90UPer025 $cpus $repet $sampl largeGE1.ini

# GE channel scenario Per=0.25 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNetG1E100UPer025 $cpus $repet $sampl largeGE1.ini

# ============================================================================================
# GE channel #2 (B=0.4 E[h1]=250) scenario Per=0.1 for 10,20,...,100 units
# ============================================================================================

# GE channel scenario Per=0.1 for 10 units (200 runs total, 4 values)
./run.sh LargeNetG2E10UPer01 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.1 for 20 units (200 runs total, 4 values)
./run.sh LargeNetG2E20UPer01 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.1 for 30 units (200 runs total, 4 values)
./run.sh LargeNetG2E30UPer01 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.1 for 40 units (200 runs total, 4 values)
./run.sh LargeNetG2E40UPer01 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.1 for 50 units (200 runs total, 4 values)
./run.sh LargeNetG2E50UPer01 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.1 for 60 units (200 runs total, 4 values)
./run.sh LargeNetG2E60UPer01 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.1 for 70 units (200 runs total, 4 values)
./run.sh LargeNetG2E70UPer01 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.1 for 80 units (200 runs total, 4 values)
./run.sh LargeNetG2E80UPer01 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.1 for 90 units (200 runs total, 4 values)
./run.sh LargeNetG2E90UPer01 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.1 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNetG2E100UPer01 $cpus $repet $sampl largeGE2.ini

# ============================================================================================
# GE channel #2 scenario Per=0.15 for 10,20,...,100 units
# ============================================================================================

# GE channel scenario Per=0.15 for 10 units (200 runs total, 4 values)
./run.sh LargeNetG2E10UPer015 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.15 for 20 units (200 runs total, 4 values)
./run.sh LargeNetG2E20UPer015 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.15 for 30 units (200 runs total, 4 values)
./run.sh LargeNetG2E30UPer015 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.15 for 40 units (200 runs total, 4 values)
./run.sh LargeNetG2E40UPer015 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.15 for 50 units (200 runs total, 4 values)
./run.sh LargeNetG2E50UPer015 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.15 for 60 units (200 runs total, 4 values)
./run.sh LargeNetG2E60UPer015 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.15 for 70 units (200 runs total, 4 values)
./run.sh LargeNetG2E70UPer015 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.15 for 80 units (200 runs total, 4 values)
./run.sh LargeNetG2E80UPer015 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.15 for 90 units (200 runs total, 4 values)
./run.sh LargeNetG2E90UPer015 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.15 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNetG2E100UPer015 $cpus $repet $sampl largeGE2.ini

# ============================================================================================
# GE channel #2 scenario Per=0.2 for 10,20,...,100 units
# ============================================================================================

# GE channel scenario Per=0.2 for 10 units (200 runs total, 4 values)
./run.sh LargeNetG2E10UPer02 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.2 for 20 units (200 runs total, 4 values)
./run.sh LargeNetG2E20UPer02 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.2 for 30 units (200 runs total, 4 values)
./run.sh LargeNetG2E30UPer02 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.2 for 40 units (200 runs total, 4 values)
./run.sh LargeNetG2E40UPer02 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.2 for 50 units (200 runs total, 4 values)
./run.sh LargeNetG2E50UPer02 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.2 for 60 units (200 runs total, 4 values)
./run.sh LargeNetG2E60UPer02 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.2 for 70 units (200 runs total, 4 values)
./run.sh LargeNetG2E70UPer02 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.2 for 80 units (200 runs total, 4 values)
./run.sh LargeNetG2E80UPer02 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.2 for 90 units (200 runs total, 4 values)
./run.sh LargeNetG2E90UPer02 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.2 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNetG2E100UPer02 $cpus $repet $sampl largeGE2.ini

# ============================================================================================
# GE channel #2 scenario Per=0.25 for 10,20,...,100 units
# ============================================================================================

# GE channel scenario Per=0.25 for 10 units (200 runs total, 4 values)
./run.sh LargeNetG2E10UPer025 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.25 for 20 units (200 runs total, 4 values)
./run.sh LargeNetG2E20UPer025 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.25 for 30 units (200 runs total, 4 values)
./run.sh LargeNetG2E30UPer025 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.25 for 40 units (200 runs total, 4 values)
./run.sh LargeNetG2E40UPer025 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.25 for 50 units (200 runs total, 4 values)
./run.sh LargeNetG2E50UPer025 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.25 for 60 units (200 runs total, 4 values)
./run.sh LargeNetG2E60UPer025 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.25 for 70 units (200 runs total, 4 values)
./run.sh LargeNetG2E70UPer025 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.25 for 80 units (200 runs total, 4 values)
./run.sh LargeNetG2E80UPer025 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.25 for 90 units (200 runs total, 4 values)
./run.sh LargeNetG2E90UPer025 $cpus $repet $sampl largeGE2.ini

# GE channel scenario Per=0.25 for 100 units (200 runs total, 4 values)
# One run takes ~9 min on i7 cpu
./run.sh LargeNetG2E100UPer025 $cpus $repet $sampl largeGE2.ini

fi

#pack everything
tar cf sim1.tar *.sim
