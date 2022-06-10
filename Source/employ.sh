#!/bin/bash

if [ "$1" != "" ]; then
    echo "Starting MPI employing"
else
    echo "Insert arguments: <machinefile> <nProcessors> <executableFile> <rows> <cols>\n"
    exit
fi
{
	rm /home/cpd2021/$3
	scp $3 node01:/home/cpd2021/
        sleep 1
	scp $3 node02:/home/cpd2021/
	sleep 1
	scp $3 node03:/home/cpd2021/
	sleep 1
} &> /dev/null

sleep 5
cp $3 /home/cpd2021/
mpirun --hostfile $1 -np $2 /home/cpd2021/$3 $4 $5
