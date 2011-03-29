#!/bin/bash
set -x 

FILE=$1
# read $FILE using the file descriptors
exec 3<&0
exec 0<$FILE
while read line
do
    TRANS_FILE=`echo $line | awk -F "," {'print $1'}`
    TRANS_NAME=`echo $line | awk -F "," {'print $2'}`
    TRANS_FILE=${TRANS_FILE%\.*}.ts
    python googlecode_upload.py -s "\"$TRANS_NAME translation\"" -p sir -u rsachetto $TRANS_FILE

done
exec 0<&3



