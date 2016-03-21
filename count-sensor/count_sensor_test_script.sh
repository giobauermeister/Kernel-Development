#!/bin/bash

while true; do
	VALUE=$(cat /sys/class/gpio/gpio39/value)
	echo -ne "Sensor value is: $VALUE"\\r
done
