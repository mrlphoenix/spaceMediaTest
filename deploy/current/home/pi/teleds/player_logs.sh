#!/bin/sh

setterm -cursor off
# you can normally leave this alone
SERVICE="TeleDSPlayer"
/bin/systemctl stop splash
/usr/bin/pkill fbi

# now for our infinite loop!
while true; do
    if pkill -0 $SERVICE > /dev/null; then
        sleep 1;
    else
	sudo cp /home/pi/teleds/log.txt /home/pi/teleds/log.backup.$(date --iso-8601=seconds)
	sudo /home/pi/teleds/updater -service 2> home/pi/teleds/updater.log
	sudo /home/pi/teleds/TeleDSPlayer 2> /home/pi/teleds/log.txt
    fi
done
