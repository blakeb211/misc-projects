#!/bin/bash -x
g++ -std=c++17 -g -O2 main.cpp tinyobj.o -lGLEW -lglfw -lGL -ldl -lm -lpthread
