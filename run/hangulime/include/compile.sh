#!/bin/bash 

arm-none-linux-gnueabi-g++ -fPIC -shared -o libhangulime.so hangulime.cpp
