#!/bin/sh

setterm -cursor off
# you can normally leave this alone
SERVICE="TeleDSPlayer"
/usr/bin/php /home/pi/teleds_git/git/splash/initialize.php > /dev/null 2>&1
/bin/systemctl stop splash > /dev/null 2>&1
/usr/bin/pkill fbi > /dev/null 2>&1

# now for our infinite loop!
while true; do
    if pkill -0 $SERVICE > /dev/null; then
        sleep 1;
    else
      sudo cp /home/pi/teleds/log.txt /home/pi/teleds/log.txt.$(date --iso-8601=seconds)
      sudo gpsd /dev/gps0 -F /var/run/gpsd.sock
      sudo /home/pi/teleds/updater -service 2> home/pi/teleds/updater.log
      sudo /home/pi/teleds/TeleDSPlayer 2> /home/pi/teleds/log.txt
    fi
done
