#!/bin/bash
# by Paul Colby (http://colby.id.au), no rights reserved ;)

PREV_TOTAL=0
PREV_IDLE=0
PRINT=0
ONE=1

while true; do
  CPU=(`sed -n 's/^cpu\s//p' /proc/stat`)
  IDLE=${CPU[3]} # Just the idle CPU time.
  TOTAL=0
  for VALUE in "${CPU[@]}"; do
    let "TOTAL=$TOTAL+$VALUE"
  done
  let "DIFF_IDLE=$IDLE-$PREV_IDLE"
  let "DIFF_TOTAL=$TOTAL-$PREV_TOTAL"
  let "DIFF_USAGE=(1000*($DIFF_TOTAL-$DIFF_IDLE)/$DIFF_TOTAL+5)/10"
  if [ $PRINT == 1 ]
  then
    echo -en "$DIFF_USAGE "
    break
  fi
  PREV_TOTAL="$TOTAL"
  PREV_IDLE="$IDLE"
  sleep 1
  PRINT=1
done

free -m | awk 'NR==2{printf "%.0f ",$3*100/$2 }'
ifconfig | grep 'RX bytes:' | cut -d: -f2 | awk '{ x += $1 } END { printf "%s ", x }'
ifconfig | grep 'TX bytes:' | cut -d: -f3 | awk '{ x += $1 } END { printf "%s", x }'
echo " END"
