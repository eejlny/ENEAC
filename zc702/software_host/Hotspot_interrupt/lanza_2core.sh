#!/bin/bash
#---------------------------

bench=HOTSPOT

data_dir=Data

echo "blocking test -----------------------------------------"
echo `date`

prog=./${bench}_DSW

for cpus in 0 1 2; do
    for block in  8 16 32 64 128 256 512 ; do
		for i in 1 2 3 4; do
	    	echo "-------------------------------------"
	    	echo ${prog} ${cpus} ${kernel} rep: ${i} blk: ${block}
	    	echo "-------------------------------------"

	    	${prog} 1024 1024 50 ./temp_1024.txt ./power_1024.txt out.txt ${cpus} 1 ${block}

	    	if [ -f ~/break ] ; then
			exit
	    	fi

		done	
    done 
done

