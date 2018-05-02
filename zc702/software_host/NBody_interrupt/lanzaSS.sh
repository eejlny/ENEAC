#!/bin/bash
#---------------------------

bench=NBODY

data_dir=Data

echo "blocking test -----------------------------------------"
echo `date`

prog=./${bench}_SS

for cpus in 2; do
    for block in  0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1; do
		for i in 1 2 3 4 5; do
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
