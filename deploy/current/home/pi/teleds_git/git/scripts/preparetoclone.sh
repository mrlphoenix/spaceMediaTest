#!/bin/bash
clear
sudo service videoplayer stop
sudo echo "{}">/home/pi/teleds/data/config.dat
sudo rm -rf /home/pi/teleds/data/video
sudo rm /home/pi/password.txt
sudo rm -f /home/pi/teleds/stat.db
sudo rm -f /home/pi/teleds/crc32.txt
sudo cp -f /home/pi/teleds_git/git/scripts/wifi.cfg.default.28 /etc/wpa_supplicant/wpa_supplicant.conf
sudo cp -f /home/pi/teleds_git/git/splash/splash.png /home/pi/teleds/splash.png
sudo chown root:root /etc/wpa_supplicant/wpa_supplicant.conf
read -n 1 -s -p "Press enter to shutdown"
printf "\n"
echo "Shutting down ..."
printf "\n"
sleep 3
sudo shutdown -h now
