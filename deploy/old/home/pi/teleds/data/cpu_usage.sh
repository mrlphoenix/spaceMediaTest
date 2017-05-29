#!/bin/bash
# by Paul Colby (http://colby.id.au), no rights reserved ;)

PREV_TOTAL=0
PREV_IDLE=0
PRINT=0
ONE=1

printf "10 10 "
ifconfig | grep 'RX bytes:' | cut -d: -f2 | awk '{ x += $1 } END { printf "%s ", x }'
ifconfig | grep 'TX bytes:' | cut -d: -f3 | awk '{ x += $1 } END { printf "%s", x }'
echo "pow 0" | cec-client -s -d 1 | grep power | cut -d : -f 2
echo " END"
