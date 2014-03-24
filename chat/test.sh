#!/bin/sh
i=0
while [ $i -le 300000 ]
do
	./chat 172.18.70.149&
	echo $i
	let i=$i+1
done
