#!/bin/bash
g++ -m64 -fPIC -DPIC -ggdb -std=c++11 -o testSocket `pkg-config --cflags --libs rSON` -pthread testSocket.cpp ../rSON_socket.cpp
