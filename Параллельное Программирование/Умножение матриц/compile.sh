#!/bin/bash

source /gpfs/soft/gcc/4.7.2/bashrc

mpicc -std="c99" -o main main.c -lm
