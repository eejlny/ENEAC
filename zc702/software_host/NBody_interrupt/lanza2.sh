#!/bin/bash
#---------------------------

bench=NBODY

data_dir=Data

echo "blocking test -----------------------------------------"
echo `date`

prog=./${bench}_DSW

for cpus in 0 1 2; do
    for block in  128 256 512 1024 2048 4096 8192; do
		for i in 1 2 3 4 5 6 7 8; do
	    	echo "-------------------------------------"
	    	echo ${prog} ${cpus} ${kernel} rep: ${i} blk: ${block}
	    	echo "-------------------------------------"

	    	${prog} ${data_dir}/runB.in ${cpus} 1 ${block}

	    	if [ -f ~/break ] ; then
			exit
	    	fi

		done	
    done 
done

