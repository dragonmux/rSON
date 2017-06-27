#!/bin/bash
g++ -m64 -fPIC -DPIC -ggdb -std=c++11 -o testRPC `pkg-config --cflags --libs rSON` -pthread testRPC.cpp ../rpc.cpp
