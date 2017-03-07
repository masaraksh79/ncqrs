#!/bin/bash

cpus=$1

repet=1250
sampl=250

# jump from per 0.1->different per values, 10 users
./run.sh ConvHo010UPer01Per $cpus $repet $sampl converge.ini conv

# jump from per 0.2->different per values, 15 users
./run.sh ConvHo015UPer01Per $cpus $repet $sampl converge.ini conv

# jump from per 0.2->different per values, 20 users
./run.sh ConvHo020UPer01Per $cpus $repet $sampl converge.ini conv

# jump from per 0.2->different per values, 10 users
./run.sh ConvHo010UPer02Per $cpus $repet $sampl converge.ini conv

# jump from per 0.2->different per values, 15 users
./run.sh ConvHo015UPer02Per $cpus $repet $sampl converge.ini conv

# jump from per 0.2->different per values, 20 users
./run.sh ConvHo020UPer02Per $cpus $repet $sampl converge.ini conv

repet=1575
sampl=225

# (m,k)-convergence hom scenarios for 5U (980 runs total, 7 values)
./run.sh ConvHo10UPer02mk $cpus $repet $sampl converge.ini conv

# (m,k)-convergence hom scenarios for 15U (980 runs total, 7 values)
./run.sh ConvHo15UPer02mk $cpus $repet $sampl converge.ini conv

# (m,k)-convergence hom scenarios for 15U (980 runs total, 7 values)
./run.sh ConvHo20UPer02mk $cpus $repet $sampl converge.ini conv
