#!/bin/bash
FACTOR=${1:-30}
DIR=$(dirname $0)
picocom -b 115200 -q --emap ignlf,igncr /dev/ttyACM0 | awk '{print $2}' | $DIR/mean_c $FACTOR | ttyplot -u 'V' -t 'Scope' -c '#' -s 3.4 -m 3.4; reset
