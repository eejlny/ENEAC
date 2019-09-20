#!/bin/bash

# Author: Kris Nikov - kris.nikov@bris.ac.uk
# Date: 20 Jun 2019
# Description: Simple script to calculate ideal execution from 1CPU and 1FPGA accelerator runs

if [[ "$#" -eq 0 ]]; then
	echo "This program requires inputs. Type -h for help." >&2
	exit 1
fi

#requires getops, but this should not be an issue since ints built in bash
while getopts ":c:f:m:s:h" opt;
do
	case $opt in
		h)
			echo "Available flags and options:" >&1
			echo "-c [DIR] -> Specify the cpu results input directory." >&1
			echo "-f [DIR] -> Specify the fpga results input directory." >&1
			echo "-m [NUM] -> Specify the ideal multiplier (1,2,3 or 4)." >&1
			echo "-s [DIR] -> Specify the smooth result output directory." >&1
			echo "Mandatory options are: -c [DIR] -f [DIR] -m [NUM] -s [DIR]" >&1
			exit 0 
			;;
		#Specify the cpu results directory
		c)
			if [[ -n  $cpu_dir ]]; then
				echo "Invalid input: option -c has already been used!" >&2
				exit 1                
			fi
			#If the directory exists, ask the user if he really wants to reuse it. I do not accept symbolic links as a save directory.
			if [[ ! -d $OPTARG ]]; then
			    	echo "Directory specified with -c flag does not exist" >&2
			    	exit 1
			else
				#directory does exists and we can analyse results
				cpu_dir=$OPTARG
			fi
			;;			
		#Specify the fpga results directory
		f)
			if [[ -n  $fpga_dir ]]; then
				echo "Invalid input: option -f has already been used!" >&2
				exit 1                
			fi
			#If the directory exists, ask the user if he really wants to reuse it. I do not accept symbolic links as a save directory.
			if [[ ! -d $OPTARG ]]; then
			    	echo "Directory specified with -f flag does not exist" >&2
			    	exit 1
			else
				#directory does exists and we can analyse results
				fpga_dir=$OPTARG
			fi
			;;			
 		#Select the number of cpu cores               
		m)
			if [[ -n $mul ]]; then
				echo "Invalid input: option -m has already been used!" >&2
				exit 1
			fi
			mul="$OPTARG"
            ;;					
		#Specify the save directory
		s)
			if [[ -n  $save_dir ]]; then
				echo "Invalid input: option -s has already been used!" >&2
				exit 1                
			fi
			#If the directory exists, ask the user if he really wants to reuse it. I do not accept symbolic links as a save directory.
			if [[ ! -d $OPTARG ]]; then
			    	echo "Directory specified with -s flag does not exist" >&2
			    	exit 1
			else
				#directory does exists and we can analyse results
				save_dir=$OPTARG
			fi
			;;			
    esac
done

#Critical checks

#Make sure we have specified a cpu results directory
if [[ -z $cpu_dir ]]; then
    	echo "Nothing to run. Expected -c flag!" >&2
    	exit 1
fi

#Make sure we have specified a fpga results directory
if [[ -z $fpga_dir ]]; then
    	echo "Nothing to run. Expected -f flag!" >&2
    	exit 1
fi

#Make sure we have specified a correct number of cpu cores
if [[ -z $mul ]]; then
    	echo "Nothing to run. Expected -m flag!" >&2
    	exit 1
else
     #Make sure the specified cpu number is a positive integer
     if ! [[ $mul =~ ^[0-9]+$ && $mul -ge 1 && $mul -le 4 ]]; then
          echo "Invalid input: specified value for multiplier number -m $mul needs to be an integer number in the range [1-4]!" >&2
          exit 1
     fi
fi

#Make sure we have specified a save directory
if [[ -z $save_dir ]]; then
    	echo "Nothing to save. Expected -s flag!" >&2
    	exit 1
fi



for cpu_results_file in $cpu_dir/*; do
	results_begin_line=$(awk -v SEP='\t' 'BEGIN{FS=SEP}{ if($1 !~ /#/){print (NR);exit} }' < "$cpu_results_file")
	header=$(awk -v SEP='\t' -v START=$((results_begin_line-1)) 'BEGIN{FS=SEP}{ if(NR==START){print $0;} }' < "$cpu_results_file")
	runnum_col=$(awk -v SEP='\t' -v START=$((results_begin_line-1)) 'BEGIN{FS=SEP}{if(NR==START){ for(i=1;i<=NF;i++){ if($i ~ /Run/) { print i; exit} } } }' < "$cpu_results_file")
	chunksize_col=$(awk -v SEP='\t' -v START=$((results_begin_line-1)) 'BEGIN{FS=SEP}{if(NR==START){ for(i=1;i<=NF;i++){ if($i ~ /Chunk/) { print i; exit} } } }' < "$cpu_results_file")
	IOCTL_col=$(awk -v SEP='\t' -v START=$((results_begin_line-1)) 'BEGIN{FS=SEP}{if(NR==START){ for(i=1;i<=NF;i++){ if($i ~ /IOCTL/) { print i; exit} } } }' < "$cpu_results_file")
	exectime_col=$(awk -v SEP='\t' -v START=$((results_begin_line-1)) 'BEGIN{FS=SEP}{if(NR==START){ for(i=1;i<=NF;i++){ if($i ~ /Execution/) { print i; exit} } } }' < "$cpu_results_file")
	cpuworkload_col=$(awk -v SEP='\t' -v START=$((results_begin_line-1)) 'BEGIN{FS=SEP}{if(NR==START){ for(i=1;i<=NF;i++){ if($i ~ /Workload/) { print i; exit} } } }' < "$cpu_results_file")
	sched_col=$(awk -v SEP='\t' -v START=$((results_begin_line-1)) 'BEGIN{FS=SEP}{if(NR==START){ for(i=1;i<=NF;i++){ if($i ~ /Scheduler/) { print i; exit} } } }' < "$cpu_results_file")
	
	cpu_chunksize=$(awk -v SEP='\t' -v START="$results_begin_line" -v COL="$chunksize_col" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$cpu_results_file")
	cpu_IOCTL=$(awk -v SEP='\t' -v START="$results_begin_line" -v COL="$IOCTL_col" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$cpu_results_file")	
	cpu_sched=$(awk -v SEP='\t' -v START="$results_begin_line" -v COL="$sched_col" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$cpu_results_file")	
	
	for fpga_results_file in $fpga_dir/*; do
    	header_test=$(awk -v SEP='\t' -v START=$((results_begin_line-1)) 'BEGIN{FS=SEP}{ if(NR==START){print $0;} }' < "$fpga_results_file")	    
	    if [[ "$header_test" != "$header" ]]; then
        	echo "Format missmatch between cpu results files and fpga results files!" >&2
    	    exit 1
	    fi
    	fpga_chunksize=$(awk -v SEP='\t' -v START="$results_begin_line" -v COL="$chunksize_col" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$fpga_results_file")
    	fpga_IOCTL=$(awk -v SEP='\t' -v START="$results_begin_line" -v COL="$IOCTL_col" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$fpga_results_file")	
    	fpga_sched=$(awk -v SEP='\t' -v START="$results_begin_line" -v COL="$sched_col" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$fpga_results_file")	    
    	if [[ "$fpga_chunksize" == "$cpu_chunksize" && "$fpga_IOCTL" == "$cpu_IOCTL" && "$fpga_sched" == "$cpu_sched" ]]; then
    	        #we found the corresponding file
    	   	    break
    	fi
    done
    full_workload=$(awk -v SEP='\t' -v START="$results_begin_line" -v COL="$cpuworkload_col" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$cpu_results_file")
    
    if [[ "$(wc -l "$cpu_results_file" | awk '{print $1}')" != "$(wc -l "$fpga_results_file" | awk '{print $1}')" ]]; then
    	echo "Number of runs missmatch between cpu results files and fpga results files!" >&2
	    exit 1
    fi
    
    
    for LINE in $(seq "$results_begin_line" 1 "$(wc -l "$cpu_results_file" | awk '{print $1}')") 
    do  
        runnum=$(awk -v SEP='\t' -v START="$LINE" -v COL="$runnum_col" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$cpu_results_file")	
        cpu_exectime=$(awk -v SEP='\t' -v START="$LINE" -v COL="$exectime_col" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$cpu_results_file")	
        cpu_throughput=$(echo "scale=2;$full_workload / $cpu_exectime" | bc )
        cpu_throughput_mul=$(echo "scale=2;($cpu_throughput*$mul)/1" | bc )

        fpga_exectime=$(awk -v SEP='\t' -v START="$LINE" -v COL="$exectime_col" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$fpga_results_file")	
        fpga_throughput=$(echo "scale=2;$full_workload / $fpga_exectime" | bc )
        fpga_throughput_mul=$(echo "scale=2;($fpga_throughput*$mul)/1" | bc )
        
        ideal_throughput=$(echo "scale=2;$cpu_throughput_mul + $fpga_throughput_mul" | bc )
        ideal_exectime=$(echo "scale=2;$full_workload / $ideal_throughput" | bc )
        
        fpga_workload=$(echo "scale=0;($ideal_exectime * $fpga_throughput_mul)/1" | bc )
        cpu_workload=$(echo "scale=0;($full_workload - $fpga_workload)/1" | bc )
        
        if [[ "$LINE" == "$results_begin_line" ]];then
            save_file="1c_0hp_1hpc_$cpu_chunksize""chunk_ideal.data"
            echo -e "$header" > "$save_dir/$save_file"
        fi
        echo -e "$runnum\t$mul\t0\t$mul\t$cpu_chunksize\t$cpu_IOCTL\t$ideal_exectime\t$cpu_workload\t$fpga_workload\t$cpu_sched\t0\t0\t0\t0" >> "$save_dir/$save_file"
    done
done
			
echo -e "====================" >&1
echo "Script Done!" >&1
echo -e "====================" >&1			
