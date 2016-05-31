#!/bin/bash  

gcc $1 -o ${1%.*} `pkg-config --cflags --libs gtk+-2.0` && ./${1%.*}
 