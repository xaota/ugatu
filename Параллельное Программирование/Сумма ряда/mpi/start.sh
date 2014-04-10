#!/bin/bash

./compile.sh

./launch.sh 1 8 10000
./launch.sh 2 16 10000
./launch.sh 4 32 10000
./launch.sh 8 64 10000
./launch.sh 16 128 10000
