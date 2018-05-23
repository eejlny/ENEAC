#!/bin/bash
#---------------------------

bench=HOTSPOT

data_dir=Data

echo "blocking test -----------------------------------------"
echo `date`


#remove driver and insert driver

rm *.txt
#rmmod my_driver
#rm /dev/my_driver
#insmod /mnt/my_driver.ko
#mknod /dev/my_driver c 100 0

prog=./${bench}_DSW

for cpus in 1 2; do
	for i in 1 2 3 4; do
	    	echo "-------------------------------------"
	    	echo ${prog} CPU_cores: ${cpus} FPGA_cores: 1 rep: ${i}
	    	echo "-------------------------------------"

	    	${prog} 1024 1024 50 ./temp_1024.dat ./power_1024.dat out.dat ${cpus} 1 dummy

	    	if [ -f ~/break ] ; then
			exit
	    	fi

    done 
done
