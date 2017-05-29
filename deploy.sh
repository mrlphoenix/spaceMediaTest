rsync -avz pi@192.168.50.53:/home/pi/teleds_git /home/nother/TeleDS/deploy/old
rsync -avz pi@192.168.50.53:/home/pi/teleds /home/nother/TeleDS/deploy/old
rm -rf /home/nother/TeleDS/deploy/old/teleds/data/video
rm -rf /home/nother/TeleDS/deploy/old/teleds/data/config.dat
