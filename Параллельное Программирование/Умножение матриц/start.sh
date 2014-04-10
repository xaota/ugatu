#!/bin/bash

./compile.sh

./launch.sh 1 0 8 10
./launch.sh 2 0 16 10
./launch.sh 4 0 32 10
./launch.sh 8 0 64 10
./launch.sh 16 0 128 10

./launch.sh 1 1 16 10
./launch.sh 3 1 32 10
./launch.sh 7 1 64 10
./launch.sh 15 1 128 10
