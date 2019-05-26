#!/bin/sh
astpath=$1
txtpath=$2
new_astpath=$3

for f in `ls $astpath/*.ast`
do
	echo "processing $f"
    astname=`basename $f`
	./packast_v2.lua "$f" "$txtpath/${astname%.*}.txt" "$new_astpath/$astname"
done