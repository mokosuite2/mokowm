#!/bin/bash

DISPLAY=":1"
if [ "$1" != "" ]; then
DISPLAY="$1"
fi

make &&
cd data && sudo make install && cd .. &&

if [ "$DISPLAY" == ":1" ]; then
DISPLAY=:0 Xephyr -ac -host-cursor -dpi 285 -screen 480x640 :1 &
sleep 3s
echo "Starting window manager"
fi

DISPLAY=$DISPLAY src/mokowm
