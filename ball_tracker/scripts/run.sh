#!/bin/bash
mkdir -p ../build
cd ../build
cmake ../ball_tracker
make
./ball_tracker