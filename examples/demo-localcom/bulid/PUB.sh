#!/bin/sh
echo "--------STARTing PUB SERVICE----------------"
export CYCLONEDDS_URI=file:///home/byd/Desktop/cyclonedds/config.xml
env | grep CYC
./Demo-Pub
