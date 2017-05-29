#!/bin/sh
sudo killall gpsd
sudo rm -f /dev/gps*
sudo rm -f /var/run/gpsd.sock
sudo sh -c "echo 0 > /sys/devices/platform/soc/3f980000.usb/buspower"
sleep 5
sudo sh -c "echo 1 > /sys/devices/platform/soc/3f980000.usb/buspower"
sleep 5
sudo gpsd /dev/gps0 -F /var/run/gpsd.sock
