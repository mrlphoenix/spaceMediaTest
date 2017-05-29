sudo sh -c "echo 0 > /sys/devices/platform/soc/3f980000.usb/buspower"
sleep 10
sudo sh -c "echo 1 > /sys/devices/platform/soc/3f980000.usb/buspower"
