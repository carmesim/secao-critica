#!/bin/bash

gcc game_of_life.c -o life -lpthread -fopenmp
./life