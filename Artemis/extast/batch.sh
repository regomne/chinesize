#!/bin/sh
astpath=$1
txtpath=$2

for f in `ls "$astpath"`
do
	echo "processing $f"
	./extast.lua "$astpath/$f" "$txtpath/${f%.*}.txt"
done