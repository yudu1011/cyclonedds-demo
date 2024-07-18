#!/bin/sh
echo "--------STARTing SUB SERVICE----------------"
export CYCLONEDDS_URI=file:///home/byd/Desktop/cyclonedds/config.xml
env | grep CYC
./Demo-Sub
