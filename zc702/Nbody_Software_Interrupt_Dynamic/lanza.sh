#!/bin/bash
#---------------------------

bench=NBODY

data_dir=Data

echo "blocking test -----------------------------------------"
echo `date`


#remove driver and insert driver

rm *.txt
rmmod my_driver
rm /dev/my_driver
insmod /mnt/my_driver.ko
mknod /dev/my_driver c 100 0

prog=./${bench}_DSW

for cpus in 0 1 2; do
	    for block in 8 16 32 64 128 256 512 1024 ; do
		for i in 1 2 3 4 ; do
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

for cpus in 1 2; do
	for block in 8 16 32 64 128 256 512 1024 ; do
		for i in 1 2 3 4 ; do
	    	echo "-------------------------------------"
	    	echo ${prog} ${cpus} ${kernel} rep: ${i} blk: ${block}
	    	echo "-------------------------------------"

	    	${prog} ${data_dir}/runB.in ${cpus} 0 ${block}

	    	if [ -f ~/break ] ; then
			exit
	    	fi
	done
    done 
done
