#!/bin/bash

source /gpfs/soft/gcc/4.7.6/bashrc

mpicc -o main main.c -lm
