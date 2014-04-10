#!/bin/bash

./writejob.sh rinat $1 $2 $3 $4

source /gpfs/soft/openmpi/1.7.2/bashrc

qsub job.pbs
