#!/bin/bash

# Author: Kris Nikov - kris.nikov@bris.ac.uk
# Date: 13 May 2019
# Description: Simple script to set up the ultrascale+ environment - load the kernel module interrupt controller drivers and the FPGA design.

if [[ "$#" -eq 0 ]]; then
	echo "This program requires inputs otherwise it uses default values. Type -h for help."
fi


#requires getops, but this should not be an issue since ints built in bash
while getopts ":n:t:drf:h" opt;
do
	case $opt in
          h)
			echo "Available flags and options:"
               echo "-n [1-4] -> Specify the number of driver files to initialize."
			echo "-d -> Select the debug option [dbg]. The divers will display helpful printk messages."
               echo "-t [VALUE] -> Specify the driver timeout value in seconds [timeout]. This is the number of seconds the driver waits for an interrupt before the thread wakes up on its own. 0 for no timeout (always wait for interrupt)."
               echo "-f [FILE] -> Specify the firmware file to load."
               echo "-r -> Reset the system to default state (remove interrupt driver modules and device files)."
			exit 0 
        		;;
          #Specify the driver debug flag
		d)
			if [[ -n $dbg ]]; then
		    		echo "Invalid input: option -d has already been used!" >&2
		    		exit 1                
			fi
		    	dbg=1
		    	;;  
          #Specify the driver timeout flag
		t)
			if [[ -n $timeout ]]; then
		    		echo "Invalid input: option -t has already been used!" >&2
		    		exit 1                
			fi
		    	timeout=$OPTARG
		    	;;
          #Specify the number of driver files
		n)
			if [[ -n $drivers ]]; then
		    		echo "Invalid input: option -n has already been used!" >&2
		    		exit 1                
			fi
		    	drivers=$OPTARG
		    	;;   
          #Specify the firmware file
		f)
			if [[ -n $firmware ]]; then
		    		echo "Invalid input: option -f has already been used!" >&2
		    		exit 1                
			fi
		    	firmware=$OPTARG
		    	;;                  
          #Reset the system to default               
          r)
			if [[ -n $rst ]]; then
		    		echo "Invalid input: option -r has already been used!" >&2
		    		exit 1                
			fi
		    	rst=1
        		;;               
		:)
			echo "Option: -$OPTARG requires an argument" >&2
			exit 1
			;;
		\?)
			echo "Invalid option: -$OPTARG" >&2
			exit 1
			;;
	esac
done

#Sanity checks
if [[ -z $rst ]]; then
     #Make sure we have specified a correct number driver files
     if [[ -z $drivers ]]; then
          echo "Nothing to load. Expected -n flag!" >&2
          exit 1
     else
          #Make sure the specified runs number is a positive integer
          if ! [[ $drivers =~ ^[0-9]+$ && $drivers -ge 1 && $drivers -le 4 ]]; then
               echo "Invalid input: specified value for number of driver files -n $drivers needs to be an integer number in the range [1-4]!" >&2
               exit 1
          fi
     fi

     #Make sure firmware file has been specified
     if [[ -z $rst && -z $firmware ]]; then
          echo "Nothing to load. Expected -f flag!" >&2
          exit 1
     else
          if ! [[ $firmware =~ .bit.bin$ ]]; then
               echo "Invalid input: specified firmware file -f $firmware needs to be in .bit.bin format!" >&2
               exit 1
          fi

          #Make sure the selected matrix file exists
          if ! [[ -e "/lib/firmware/$firmware" ]]; then
               echo "-f $firmware does not exist. Please enter a valid firmware file from the /lib/fiwmware directory!" >&2
               exit 1
          fi
          
          if ! [[ -s "/lib/firmware/$firmware" ]]; then
               echo "Invalid input: specified firmware file -f $firmware is empty!" >&2
               exit 1
          fi
     fi
     
     if [[ -z $dbg ]]; then
          dbg=0
          echo "Using default value for dbg -> $dbg (no debug information)!"     
     else
          echo "User specified -d flag selected."
          echo "Driver debug information enabled!"               
     fi

     if [[ -z $timeout ]]; then
          timeout=0
          echo "Using default value for driver timeout -> $timeout seconds (no driver wait queue timeout, always wait for interrupt to wake thread)!"     
     else
          if ! [[ $timeout =~ ^[0-9]+$ && $timeout -ge 1 ]]; then
               echo "Invalid input: specified value for driver timeout -t $timeout needs to be a positive integer number!" >&2
               exit 1
          else
               echo "User specified -t flag selected."
               echo "Driver ioctl timeout enabled using specified value of $timeout seconds!"
          fi
     fi
else
     if [[ -n $drivers || -n $firmware || -n $dbg || -n $timeout ]]; then
          echo "Please do not use -r flag with any other system setup flags!" >&2
          exit 1
     fi
fi

if [[ -n $rst ]]; then
     echo "User specified -r flag selected."
     echo "Resetting the system to default state!" 
     rmmod intgendriver1 2> /dev/null
     rmmod intgendriver2 2> /dev/null
     rmmod intgendriver3 2> /dev/null
     rmmod intgendriver4 2> /dev/null
     echo "Removed interrupt driver kernel modules!"
     #NB: There is a problem with rmmod which does not free the driver major number. Currently there isn't an issue with this since the drivers have explicit major numbers, so initialization is consistent, but if there is a problem in the future I might need to come back to this. There is little documentation and solutions last time I checked -> April 2019.
     rm "/dev/intgendriver1"
     rm "/dev/intgendriver2"
     rm "/dev/intgendriver3"
     rm "/dev/intgendriver4"
     echo "Removed interrupt driver device files!" 
else
     echo "Preparing environment!"
     mount /dev/mmcblk0p1 /media/card/
     echo $firmware > /sys/class/fpga_manager/fpga0/firmware #load firmware to FPGA
     [[ $drivers -ge 1 ]] && insmod /media/card/intgendriver1.ko DBG=$dbg TIMEOUT=$timeout
     [[ $drivers -ge 2 ]] && insmod /media/card/intgendriver2.ko DBG=$dbg TIMEOUT=$timeout
     [[ $drivers -ge 3 ]] && insmod /media/card/intgendriver3.ko DBG=$dbg TIMEOUT=$timeout
     [[ $drivers -ge 4 ]] && insmod /media/card/intgendriver4.ko DBG=$dbg TIMEOUT=$timeout
     echo "Loaded interrupt driver kernel modules!" 
     [[ $drivers -ge 1 ]] && mknod "/dev/intgendriver1" c 100 0
     [[ $drivers -ge 2 ]] && mknod "/dev/intgendriver2" c 101 0
     [[ $drivers -ge 3 ]] && mknod "/dev/intgendriver3" c 102 0
     [[ $drivers -ge 4 ]] && mknod "/dev/intgendriver4" c 103 0 
     echo "Created interrupt driver device files!" 
     umount /media/card/
fi

echo "Done! :)"
