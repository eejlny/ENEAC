#!/bin/bash

if [[ "$#" -eq 0 ]]; then
	echo "This program requires inputs. Type -h for help." >&2
	exit 1
fi

#requires getops, but this should not be an issue since ints built in bash
while getopts ":d:s:h" opt;
do
	case $opt in
		h)
			echo "Available flags and options:" >&1
			echo "-d [DIR] -> Specify the result input directory." >&1
			echo "-s [DIR] -> Specify the smooth result output directory." >&1
			echo "Mandatory options are: -d [DIR] -s [DIR]" >&1
			exit 0 
			;;
		#Specify the result directory
		d)
			if [[ -n  $RESULTS_DIR ]]; then
				echo "Invalid input: option -d has already been used!" >&2
				exit 1                
			fi
			#If the directory exists, ask the user if he really wants to reuse it. I do not accept symbolic links as a save directory.
			if [[ ! -d $OPTARG ]]; then
			    	echo "Directory specified with -d flag does not exist" >&2
			    	exit 1
			else
				#directory does exists and we can analyse results
				RESULTS_DIR=$OPTARG
			fi
			;;			
		#Specify the save directory
		s)
			if [[ -n  $SAVE_DIR ]]; then
				echo "Invalid input: option -s has already been used!" >&2
				exit 1                
			fi
			#If the directory exists, ask the user if he really wants to reuse it. I do not accept symbolic links as a save directory.
			if [[ ! -d $OPTARG ]]; then
			    	echo "Directory specified with -s flag does not exist" >&2
			    	exit 1
			else
				#directory does exists and we can analyse results
				SAVE_DIR=$OPTARG
			fi
			;;			
    esac
done

for RESULTS_FILE in $RESULTS_DIR/*; do
	RESULTS_BEGIN_LINE=$(awk -v SEP='\t' 'BEGIN{FS=SEP}{ if($1 !~ /#/){print (NR);exit} }' < "$RESULTS_FILE")
	HEADER=$(awk -v START="$BEGIN_LINE" -v SEP='\t' -v START=$((RESULTS_BEGIN_LINE-1)) 'BEGIN{FS=SEP}{ if(NR==START){print $0;} }' < "$RESULTS_FILE")
	EXEC_TIME_COL=$(awk -v SEP='\t' -v START=$((RESULTS_BEGIN_LINE-1)) 'BEGIN{FS=SEP}{if(NR==START){ for(i=1;i<=NF;i++){ if($i ~ /Execution/) { print i; exit} } } }' < "$RESULTS_FILE")
	echo -e "$HEADER" > "$SAVE_DIR/"$(basename $RESULTS_FILE .data)"_smooth.data"
	#Find min and max exec time
	min=0
	max=0
	for LINE in $(seq "$RESULTS_BEGIN_LINE" 1 "$(wc -l "$RESULTS_FILE" | awk '{print $1}')") 
	do
	    EXEC_TIME=$(awk -v SEP='\t' -v START="$LINE" -v COL="$EXEC_TIME_COL" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$RESULTS_FILE")
        if [[ $min == 0 ]]; then
            min=$EXEC_TIME
        else
            (( $(echo "$EXEC_TIME < $min" |bc -l) )) && min=$EXEC_TIME
        fi
        if [[ $max == 0 ]]; then
            max=$EXEC_TIME
        else
            (( $(echo "$EXEC_TIME > $max" |bc -l) )) && max=$EXEC_TIME
        fi
	done
	#Remove min and max from file
	for LINE in $(seq "$RESULTS_BEGIN_LINE" 1 "$(wc -l "$RESULTS_FILE" | awk '{print $1}')") 
	do
	    EXEC_TIME=$(awk -v SEP='\t' -v START="$LINE" -v COL="$EXEC_TIME_COL" 'BEGIN{FS=SEP}{if(NR==START){print $COL;exit} }' < "$RESULTS_FILE")
        if ! [[ $EXEC_TIME == $min || $EXEC_TIME == $max ]]; then
    		DATA=$(awk -v SEP='\t' -v START="$LINE" 'BEGIN{FS=SEP}{if(NR==START){ print $0} }' < "$RESULTS_FILE") 
    		echo -e "$DATA" >> "$SAVE_DIR/"$(basename $RESULTS_FILE .data)"_smooth.data"
        fi		
    done
done
			
echo -e "====================" >&1
echo "Script Done!" >&1
echo -e "====================" >&1			
