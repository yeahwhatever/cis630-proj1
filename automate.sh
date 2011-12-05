#!/bin/bash

IP=localhost
PORT=5001

for TRIAL in 1 2 3 4 5 
do

for NUMPROCS in 2 4 8
do

##################################################################
###  TESTS########################################################
##################################################################
echo "mpirun -np $NUMPROCS server $IP $PORT > s_test.out &"
mpirun -np $NUMPROCS server $IP $PORT > s_test.out &
sleep 2
SECONDS=0
echo "./client $IP $PORT < inputs/test > test.out"
./client $IP $PORT < inputs/test > test.out
echo "Execution time: ~$SECONDS seconds" >> test.out
killall -9 server

#Run every trial w00t

##################################################################
###  500 x 500 with 5, 10, 20 heatpoints #########################
##################################################################
SECONDS=0
mpirun -np $NUMPROCS server $IP $PORT > s_500_500_5_$TRIAL.$NUMPROCS.out &
sleep 2
echo "./client $IP $PORT < inputs/input1 > 500_500_5_$TRIAL.$NUMPROCS.out"
./client $IP $PORT < inputs/input1 > 500_500_5_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 500_500_5_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_500_500_5_$TRIAL.$NUMPROCS.out &
sleep 2

SECONDS=0
echo "./client $IP $PORT < inputs/input2 > 500_500_10_$TRIAL.$NUMPROCS.out"
./client $IP $PORT < inputs/input2 > 500_500_10_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 500_500_10_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_500_500_5_$TRIAL.$NUMPROCS.out &
sleep 2

SECONDS=0
./client $IP $PORT < inputs/input3 > 500_500_20_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 500_500_20_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_500_500_5_$TRIAL.$NUMPROCS.out &
sleep 2

##################################################################
###  1000 x 1000 with 5, 10, 20 heatpoints #######################
##################################################################
SECONDS=0
./client $IP $PORT < inputs/input4 > 1000_1000_5_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 1000_1000_5_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_1000_1000_5_$TRIAL.$NUMPROCS.out &
sleep 2

SECONDS=0
./client $IP $PORT < inputs/input5 > 1000_1000_10_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 1000_1000_10_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_1000_1000_5_$TRIAL.$NUMPROCS.out &
sleep 2

SECONDS=0
./client $IP $PORT < inputs/input6 > 1000_1000_20_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 1000_1000_20_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_1000_1000_5_$TRIAL.$NUMPROCS.out &
sleep 2

##################################################################
###  2000 x 2000 with 5, 10, 20 heatpoints #######################
##################################################################

SECONDS=0
./client $IP $PORT < inputs/input7 > 2000_2000_5_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 2000_2000_5_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_2000_2000_5_$TRIAL.$NUMPROCS.out &
sleep 2

SECONDS=0
./client $IP $PORT < inputs/input8 > 2000_2000_10_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 2000_2000_10_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_2000_2000_5_$TRIAL.$NUMPROCS.out &
sleep 2

SECONDS=0
./client $IP $PORT < inputs/input9 > 2000_2000_20_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 2000_2000_20_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_2000_2000_5_$TRIAL.$NUMPROCS.out &
sleep 2

##################################################################
###  500 x 2000 with 5, 10, 20 heatpoints #######################
##################################################################

SECONDS=0
./client $IP $PORT < inputs/input10 > 500_2000_5_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 500_2000_5_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_500_2000_5_$TRIAL.$NUMPROCS.out &
sleep 2

SECONDS=0
./client $IP $PORT < inputs/input11 > 500_2000_10_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 500_2000_10_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_500_2000_5_$TRIAL.$NUMPROCS.out &
sleep 2

SECONDS=0
./client $IP $PORT < inputs/input12 > 500_2000_20_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 500_2000_20_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_500_2000_5_$TRIAL.$NUMPROCS.out &
sleep 2

##################################################################
###  2000 x 500 with 5, 10, 20 heatpoints #######################
##################################################################

SECONDS=0
./client $IP $PORT < inputs/input13 > 2000_500_5_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 2000_500_5_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_2000_500_5_$TRIAL.$NUMPROCS.out &
sleep 2

SECONDS=0
./client $IP $PORT < inputs/input14 > 2000_500_10_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 2000_500_10_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_2000_500_5_$TRIAL.$NUMPROCS.out &
sleep 2

SECONDS=0
./client $IP $PORT < inputs/input15 > 2000_500_20_$TRIAL.$NUMPROCS.out
echo "Execution time: ~$SECONDS seconds" >> 2000_500_20_$TRIAL.$NUMPROCS.out
killall -9 server
mpirun -np $NUMPROCS server $IP $PORT > s_2000_500_5_$TRIAL.$NUMPROCS.out &
sleep 2

killall -9 

done
done
