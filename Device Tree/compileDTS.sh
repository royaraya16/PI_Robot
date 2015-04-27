#!/bin/bash

dtc -O dtb -o /lib/firmware/PI-ROBOT-00A0.dtbo -b 0 -@ PI-ROBOT-00A0.dts

echo "Compliling DONE"
