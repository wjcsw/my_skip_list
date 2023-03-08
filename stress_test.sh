#!/bin/bash

g++ stress/stress.cpp -o stress_test -pthread
./stress_test 100000
./stress_test 500000
./stress_test 1000000
