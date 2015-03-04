#!/bin/bash

# This little script ensures that the kernel I2C drivers are loaded, and it
# changes permissions on /dev/i2c-2 so you can run the demos as a regular
# user

sudo modprobe i2c-dev
sleep 0.1
#Cambiar el numero de i2c dependiendo del que se este usando
sudo chmod 666 /dev/i2c-1


