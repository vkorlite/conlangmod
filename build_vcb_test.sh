#!/bin/bash

export CONLANGMOD_REC_DIR="$PWD/data/"

cmake -B build
cmake --build build
build/clm_vcb_test

