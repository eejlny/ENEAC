#!/usr/bin/env python

# Adapted from: Antonio Vilches
# Revised by:   Kris Nikov

##############################################################################
# Version:      3.0; 18 Apr 2019
# Description:  Plotting script for ENEAC
##############################################################################

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from matplotlib.ticker import ScalarFormatter
from matplotlib.ticker import MaxNLocator
from collections import namedtuple
import matplotlib.patches as mpatches
from operator import itemgetter
import os
import copy
import matplotlib.ticker
import matplotlib.lines as mlines
from matplotlib.legend_handler import HandlerPatch
from matplotlib.legend_handler import HandlerLine2D
from matplotlib.pyplot import cm


class ArrowHandler(HandlerLine2D):
    def create_artists(self, legend, orig_handle, xdescent, ydescent, width, height, fontsize, trans):
        xdata, xdata_marker = self.get_xdata(legend, xdescent, ydescent, width, height, fontsize)
        ydata = ((height - ydescent) / 2.) * np.ones(xdata.shape, float)
        legline = mpatches.FancyArrow(xdata[0],ydata[0],xdata[1]-5,0, width=1, head_length=5, head_width=5, color=orig_handle.get_color())
        #self.update_prop(legline, orig_handle, legend)
        legline.set_transform(trans)
        return [legline]

def f(): raise Exception("Found exit()")

import sys, getopt

def main(argv):
    plottype = 0
    inputfile = [] #use array for input file
    inputfile_header = ''
    inputdir = []
    outputfile = ''
    
    runs = []
    cpu_cores = []
    fpga_hpacc = []
    fpga_hpcacc = []
    chunk_size = []
    ioctl_flag = []
    exec_time = []
    cpu_rows = []
    fpga_rows = []
    sched_type = []
    cpu_power = []
    fpga_power = []
    cpu_energy = []
    fpga_energy = []
    aggregated_results = []
    
    ideal_multiplier = 0
    ideal_inputfile =[]
    ideal_inputdir = []
    ideal_runs = []
    ideal_cpu_cores = []
    ideal_fpga_hpacc = []
    ideal_fpga_hpcacc = []
    ideal_chunk_size = []
    ideal_ioctl_flag = []
    ideal_exec_time = []
    ideal_cpu_rows = []
    ideal_fpga_rows = []
    ideal_sched_type = []
    ideal_cpu_power = []
    ideal_fpga_power = []
    ideal_cpu_energy = []
    ideal_fpga_energy = []
    ideal_aggregated_results = []
    
    multihap_inputfile = []
    multihap_runs = []
    multihap_cpu_cores = []
    multihap_fpga_hpacc = []
    multihap_fpga_hpcacc = []
    multihap_chunk_size = []
    multihap_ioctl_flag = []
    multihap_exec_time = []
    multihap_cpu_rows = []
    multihap_fpga_rows = []
    multihap_sched_type = []
    multihap_cpu_power = []
    multihap_fpga_power = []
    multihap_cpu_energy = []
    multihap_fpga_energy = []
    multihap_aggregated_results = []
        
    try:
        opts, args = getopt.getopt(argv,"hp:i:d:e:j:m:u:o:",["ptype=","ifile=","idir=","idfile=","iddir=","idmul=","mhfile=","ofile="])
    except getopt.GetoptError:
        print 'Error: use -h option to see usage!'
        sys.exit(2)
    
    for opt, arg in opts:
        if opt == '-h':
            print 'Usage: -p <plot type> -i <input file> -d <input directory> -e <ideal input file> -j <ideal input directory> -m <ideal multiplier> -u <multihap input file> -o <output file>'
            sys.exit(2)
        elif opt in ("-p", "--ptype"):
            if plottype != 0:
		        print 'Error in option <-p ' + arg + '>: -p flag has already been used! First usage is: <-p ' + plottype + '>'
		        sys.exit(2)
            else:
                plottype = int(arg)
        elif opt in ("-i", "--ifile"):
            try:
                fileopentest = open(arg, 'r')
                if arg in inputfile:
                    print 'Error in option <-i ' + arg + '>: file <' + arg + '> already selected. Please enter a different input file.'
                    fileopentest.close()
                    sys.exit(2)
                else:
                    if fileopentest.readline() != 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tCPU Power[W]\tFPGA Power[W]\tCPU Energy[J]\tFPGA Energy[J]\n':
                        print 'Error in option <-i ' + arg + '>: file <' + arg + '> is not the correct format or is empty. Please enter a valid input file.'
                        print 'The results file should start with the following header, followed by the data:'
                        print 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tCPU Power[W]\tFPGA Power[W]\tCPU Energy[J]\tFPGA Energy[J]'
                        print 'NB: This script DOES NOT currently have a way to determine if the data format is correct so please use the proper data collection tool in order to not break the plots.'
                        fileopentest.close()
                        sys.exit(2)
                    else:
                        fileopentest.close()
                        inputfile.append(arg)
            except IOError, e:
		            print 'Error in option <-i ' + arg + '>: file <' + arg + '> does not exist. Please enter a valid input file.'
		            sys.exit(2)
        elif opt in ("-d", "--idir"):
            #Check if directory is valid
            if os.path.isdir(arg):
                #Check if directory is already selected
                if arg in inputdir:
                    print 'Error in option <-d ' + arg + '>: directory <' + arg + '> already selected. Please enter a different input directory.'
                    sys.exit(2)
                else:
                    #Check if directory is empty:
                    tempinputfilelist = os.listdir(arg)
                    if tempinputfilelist == []:
                        print 'Error in option <-d ' + arg + '>: directory <' + arg + '> is empty. Please enter a valid input directory.'
                        sys.exit(2)
                    else:
                        for tempinputfile in tempinputfilelist:
                            #Check if directory contains already selected files                        
                            if arg+'/'+tempinputfile in inputfile:
                                print 'Error in option <-d ' + arg + '>: directory <' + arg + '> contains already selected files. Please enter a different input directory.'
                                sys.exit(2)
                            else:
                                #Check if directory contains illegal files                                                    
                                fileopentest = open(arg+'/'+tempinputfile, 'r')
                                if fileopentest.readline() != 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tCPU Power[W]\tFPGA Power[W]\tCPU Energy[J]\tFPGA Energy[J]\n':
                                    print 'Error in option <-d ' + arg + '>: directory <' + arg + '> contains illegal input files (' + tempinputfile + '). Please enter a valid input directory.'
                                    print 'The results file should start with the following header, followed by the data:'
                                    print 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tCPU Power[W]\tFPGA Power[W]\tCPU Energy[J]\tFPGA Energy[J]'
                                    print 'NB: This script DOES NOT currently have a way to determine if the data format is correct so please use the proper data collection tool in order to not break the plots.'
                                    fileopentest.close()
                                    sys.exit(2)
                                else:
                                    inputdir.append(arg)                                
                                    fileopentest.close()
                                    inputfile.append(arg+'/'+tempinputfile)
            else:
	            print 'Error in option <-d ' + arg + '>: directory <' + arg + '> does not exist. Please enter a valid input directory.'
	            sys.exit(2)
        elif opt in ("-e", "--idfile"):
            try:
                fileopentest = open(arg, 'r')
                if arg in ideal_inputfile:
                    print 'Error in option <-e ' + arg + '>: file <' + arg + '> already selected. Please enter a different ideal input file.'
                    fileopentest.close()
                    sys.exit(2)
                else:
                    if fileopentest.readline() != 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tCPU Power[W]\tFPGA Power[W]\tCPU Energy[J]\tFPGA Energy[J]\n':
                        print 'Error in option <-e ' + arg + '>: file <' + arg + '> is not the correct format or is empty. Please enter a valid input file.'
                        print 'The results file should start with the following header, followed by the data:'
                        print 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tCPU Power[W]\tFPGA Power[W]\tCPU Energy[J]\tFPGA Energy[J]'
                        print 'NB: This script DOES NOT currently have a way to determine if the data format is correct so please use the proper data collection tool in order to not break the plots.'
                        fileopentest.close()
                        sys.exit(2)
                    else:
                        fileopentest.close()
                        ideal_inputfile.append(arg)
            except IOError, e:
		            print 'Error in option <-e ' + arg + '>: file <' + arg + '> does not exist. Please enter a valid input file.'
		            sys.exit(2)	            
        elif opt in ("-j", "--iddir"):
            #Check if directory is valid
            if os.path.isdir(arg):
                #Check if directory is already selected
                if arg in ideal_inputdir:
                    print 'Error in option <-j ' + arg + '>: directory <' + arg + '> already selected. Please enter a different ideal input directory.'
                    sys.exit(2)
                else:
                    #Check if directory is empty:
                    tempinputfilelist = os.listdir(arg)
                    if tempinputfilelist == []:
                        print 'Error in option <-j ' + arg + '>: directory <' + arg + '> is empty. Please enter a valid input directory.'
                        sys.exit(2)
                    else:
                        for tempinputfile in tempinputfilelist:
                            #Check if directory contains already selected files                        
                            if arg+'/'+tempinputfile in ideal_inputfile:
                                print 'Error in option <-j ' + arg + '>: directory <' + arg + '> contains already selected files. Please enter a different input directory.'
                                sys.exit(2)
                            else:
                                #Check if directory contains illegal files                                                    
                                fileopentest = open(arg+'/'+tempinputfile, 'r')
                                if fileopentest.readline() != 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tCPU Power[W]\tFPGA Power[W]\tCPU Energy[J]\tFPGA Energy[J]\n':
                                    print 'Error in option <-j ' + arg + '>: directory <' + arg + '> contains illegal input files (' + tempinputfile + '). Please enter a valid input directory.'
                                    print 'The results file should start with the following header, followed by the data:'
                                    print 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tCPU Power[W]\tFPGA Power[W]\tCPU Energy[J]\tFPGA Energy[J]'
                                    print 'NB: This script DOES NOT currently have a way to determine if the data format is correct so please use the proper data collection tool in order to not break the plots.'
                                    fileopentest.close()
                                    sys.exit(2)
                                else:
                                    ideal_inputdir.append(arg)                                
                                    fileopentest.close()
                                    ideal_inputfile.append(arg+'/'+tempinputfile)
            else:
	            print 'Error in option <-j ' + arg + '>: directory <' + arg + '> does not exist. Please enter a valid input directory.'
	            sys.exit(2)
        elif opt in ("-m", "--idmul"):
            if ideal_multiplier != 0:
		        print 'Error in option <-m ' + arg + '>: -m flag has already been used! First usage is: <-m ' + ideal_multiplier + '>'
		        sys.exit(2)
            else:
                ideal_multiplier = int(arg)		            
        elif opt in ("-u", "--mhfile"):
            try:
                fileopentest = open(arg, 'r')
                if arg in multihap_inputfile:
                    print 'Error in option <-u ' + arg + '>: file <' + arg + '> already selected. Please enter a different multihap input file.'
                    fileopentest.close()
                    sys.exit(2)
                else:
                    if fileopentest.readline() != 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tCPU Power[W]\tFPGA Power[W]\tCPU Energy[J]\tFPGA Energy[J]\n':
                        print 'Error in option <-e ' + arg + '>: file <' + arg + '> is not the correct format or is empty. Please enter a valid input file.'
                        print 'The results file should start with the following header, followed by the data:'
                        print 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tCPU Power[W]\tFPGA Power[W]\tCPU Energy[J]\tFPGA Energy[J]'
                        print 'NB: This script DOES NOT currently have a way to determine if the data format is correct so please use the proper data collection tool in order to not break the plots.'
                        fileopentest.close()
                        sys.exit(2)
                    else:
                        fileopentest.close()
                        multihap_inputfile.append(arg)
            except IOError, e:
		            print 'Error in option <-eu' + arg + '>: file <' + arg + '> does not exist. Please enter a valid input file.'
		            sys.exit(2)	                        
        elif opt in ("-o", "--ofile"):
            if outputfile != '':
		        print 'Error in option <-o ' + arg + '>: -o flag has already been used! First usage is: <-o ' + outputfile + '>'
		        sys.exit(2)
            else:
                try:
                    fileopentest = open(arg, 'r')
                    print 'Filename selected in option <-o ' + arg + '>: file <' + arg + '> already exists. Would you like to overwrite existing file?'
                    while True:
                        outputfile_userinput = raw_input("Please specify [Y/N]: ")
                        if outputfile_userinput == 'Y' or outputfile_userinput == 'y':
                            print 'Overwriting specified file.'
                            outputfile = arg
                            fileopentest.close()
                            break
                        elif outputfile_userinput == 'N' or outputfile_userinput == 'n':
                            print 'Please restart the script with a different output file argument.'
                            fileopentest.close()
                            quit()
                        else:
                            print 'Invalid input!'
                except IOError, e:
                    outputfile = arg
               
    #Checks
    if ideal_multiplier > 0 and ideal_inputfile == [] and ideal_inputdir == []:
        print 'Error: option <-m ' + ideal_multiplier + '> used, but no file '
        sys.exit(2)
    if ideal_multiplier == 0 and ( ideal_inputfile != [] or ideal_inputdir != []) :
        print 'Error: ideal input file/directory selected but option <-m INTEGER> is unspecified.'
        sys.exit(2)
        
    print 'Plot type is ' + str(plottype)
    print 'Input file is ' + str(inputfile)[1:-1]
    print 'Output file is ' + outputfile
    if ideal_multiplier > 0:
        print 'Ideal input file is ' + str(ideal_inputfile)[1:-1]
        print 'Ideal multiplier is ' + str(ideal_multiplier)
    if multihap_inputfile != []:
        print 'Multihap input file is ' + str(multihap_inputfile)[1:-1]    
    
    #Data processing
    for num, infile in enumerate(inputfile):
        with open(infile,'r') as inputfile_read:
            runs.append(0)
            cpu_cores.append(-1)
            fpga_hpacc.append(-1)
            fpga_hpcacc.append(-1)
            chunk_size.append(0)
            ioctl_flag.append('')
            exec_time.append(0)
            cpu_rows.append(0)
            fpga_rows.append(0)
            sched_type.append('')
            cpu_power.append(0)
            fpga_power.append(0)
            cpu_energy.append(0)
            fpga_energy.append(0)            
            
            for curline in inputfile_read:
                # check if the current linestarts with "#"
                if curline.startswith("R"):
                    if inputfile_header == '':
                        inputfile_header = curline
                else:
                    runs[num] += 1
                    if cpu_cores[num] == -1:
                        cpu_cores[num] = int(curline.split('\t')[1])
                    if fpga_hpacc[num] == -1:
                        fpga_hpacc[num] = int(curline.split('\t')[2])
                    if fpga_hpcacc[num] == -1:
                        fpga_hpcacc[num] = int(curline.split('\t')[3])   
                    if chunk_size[num] == 0:
                        chunk_size[num] = int(curline.split('\t')[4]) 
                    if ioctl_flag[num] == '':
                        ioctl_flag[num] = curline.split('\t')[5]
                    exec_time[num] += float(curline.split('\t')[6])                       
                    cpu_rows[num] += int(curline.split('\t')[7])  
                    fpga_rows[num] += int(curline.split('\t')[8])                            
                    if sched_type[num] == '':
                        sched_type[num] = curline.split('\t')[9]    
                    cpu_power[num] += float(curline.split('\t')[10])
                    fpga_power[num] += float(curline.split('\t')[11])
                    cpu_energy[num] += float(curline.split('\t')[12])
                    fpga_energy[num] += float(curline.split('\t')[13])

            exec_time[num] /= runs[num]
            cpu_power[num] /= runs[num]
            fpga_power[num] /= runs[num]
            cpu_energy[num] /= runs[num]
            fpga_energy[num] /= runs[num]
            if cpu_rows[num] != 0:
                cpu_rows[num] /= runs[num]
            if fpga_rows[num] != 0:
                fpga_rows[num] /= runs[num]    
                     
            aggregated_results.append(str(runs[num])+'\t'+str(cpu_cores[num])+'\t'+str(fpga_hpacc[num])+'\t'+str(fpga_hpcacc[num])+'\t'+str(chunk_size[num])+'\t'+ioctl_flag[num]+'\t'+str(exec_time[num])+'\t'+str(cpu_rows[num])+'\t'+str(fpga_rows[num])+'\t'+sched_type[num]+'\t'+str(cpu_power[num])+'\t'+str(fpga_power[num])+'\t'+str(cpu_energy[num])+'\t'+str(fpga_energy[num]))    
            
            
    if ideal_multiplier > 0:
        #Ideal data processing
        for num, infile in enumerate(ideal_inputfile):
            with open(infile,'r') as ideal_inputfile_read:
                ideal_runs.append(0)
                ideal_cpu_cores.append(-1)
                ideal_fpga_hpacc.append(-1)
                ideal_fpga_hpcacc.append(-1)
                ideal_chunk_size.append(0)
                ideal_ioctl_flag.append('')
                ideal_exec_time.append(0)
                ideal_cpu_rows.append(0)
                ideal_fpga_rows.append(0)
                ideal_sched_type.append('')
                ideal_cpu_power.append(0)
                ideal_fpga_power.append(0)
                ideal_cpu_energy.append(0)
                ideal_fpga_energy.append(0)            
                
                for curline in ideal_inputfile_read:
                    # check if the current linestarts with "#"
                    if curline.startswith("R"):
                        if inputfile_header == '':
                            inputfile_header = curline
                    else:
                        ideal_runs[num] += 1
                        if ideal_cpu_cores[num] == -1:
                            ideal_cpu_cores[num] = int(curline.split('\t')[1])
                        if ideal_fpga_hpacc[num] == -1:
                            ideal_fpga_hpacc[num] = int(curline.split('\t')[2])
                        if ideal_fpga_hpcacc[num] == -1:
                            ideal_fpga_hpcacc[num] = int(curline.split('\t')[3])   
                        if ideal_chunk_size[num] == 0:
                            ideal_chunk_size[num] = int(curline.split('\t')[4]) 
                        if ideal_ioctl_flag[num] == '':
                            ideal_ioctl_flag[num] = curline.split('\t')[5]
                        ideal_exec_time[num] += float(curline.split('\t')[6])                       
                        ideal_cpu_rows[num] += int(curline.split('\t')[7])  
                        ideal_fpga_rows[num] += int(curline.split('\t')[8])                            
                        if ideal_sched_type[num] == '':
                            ideal_sched_type[num] = curline.split('\t')[9]    
                        ideal_cpu_power[num] += float(curline.split('\t')[10])
                        ideal_fpga_power[num] += float(curline.split('\t')[11])
                        ideal_cpu_energy[num] += float(curline.split('\t')[12])
                        ideal_fpga_energy[num] += float(curline.split('\t')[13])

                ideal_exec_time[num] /= ideal_runs[num]
                ideal_cpu_power[num] /= ideal_runs[num]
                ideal_fpga_power[num] /= ideal_runs[num]
                ideal_cpu_energy[num] /= ideal_runs[num]
                ideal_fpga_energy[num] /= ideal_runs[num]
                if ideal_cpu_rows[num] != 0:
                    ideal_cpu_rows[num] /= ideal_runs[num]
                if ideal_fpga_rows[num] != 0:
                    ideal_fpga_rows[num] /= ideal_runs[num]
                       
                ideal_exec_time[num] /= ideal_multiplier
                ideal_cpu_power[num] *= ideal_multiplier
                ideal_fpga_power[num] *= ideal_multiplier
                ideal_cpu_energy[num] *= ideal_multiplier
                ideal_fpga_energy[num] *= ideal_multiplier                
                if ideal_cpu_cores[num] != 0:
                    ideal_cpu_cores[num] *= ideal_multiplier
                if ideal_fpga_hpacc[num] != 0:
                    ideal_fpga_hpacc[num] *= ideal_multiplier
                if ideal_fpga_hpcacc[num] != 0:
                    ideal_fpga_hpcacc[num] *= ideal_multiplier                    
                         
                ideal_aggregated_results.append(str(ideal_runs[num])+'\t'+str(ideal_cpu_cores[num])+'\t'+str(ideal_fpga_hpacc[num])+'\t'+str(ideal_fpga_hpcacc[num])+'\t'+str(ideal_chunk_size[num])+'\t'+ideal_ioctl_flag[num]+'\t'+str(ideal_exec_time[num])+'\t'+str(ideal_cpu_rows[num])+'\t'+str(ideal_fpga_rows[num])+'\t'+ideal_sched_type[num]+'\t'+str(ideal_cpu_power[num])+'\t'+str(ideal_fpga_power[num])+'\t'+str(ideal_cpu_energy[num])+'\t'+str(ideal_fpga_energy[num]))
            
    
    if multihap_inputfile != []:
        #Multihap data processing
        for num, infile in enumerate(multihap_inputfile):
            with open(infile,'r') as multihap_inputfile_read:
                multihap_runs.append(0)
                multihap_cpu_cores.append(-1)
                multihap_fpga_hpacc.append(-1)
                multihap_fpga_hpcacc.append(-1)
                multihap_chunk_size.append(0)
                multihap_ioctl_flag.append('')
                multihap_exec_time.append(0)
                multihap_cpu_rows.append(0)
                multihap_fpga_rows.append(0)
                multihap_sched_type.append('')
                multihap_cpu_power.append(0)
                multihap_fpga_power.append(0)
                multihap_cpu_energy.append(0)
                multihap_fpga_energy.append(0)            
                
                for curline in multihap_inputfile_read:
                    # check if the current linestarts with "#"
                    if curline.startswith("R"):
                        if inputfile_header == '':
                            inputfile_header = curline
                    else:
                        multihap_runs[num] += 1
                        if multihap_cpu_cores[num] == -1:
                            multihap_cpu_cores[num] = int(curline.split('\t')[1])
                        if multihap_fpga_hpacc[num] == -1:
                            multihap_fpga_hpacc[num] = int(curline.split('\t')[2])
                        if multihap_fpga_hpcacc[num] == -1:
                            multihap_fpga_hpcacc[num] = int(curline.split('\t')[3])   
                        multihap_chunk_size[num] += int(curline.split('\t')[4]) 
                        if multihap_ioctl_flag[num] == '':
                            multihap_ioctl_flag[num] = curline.split('\t')[5]
                        multihap_exec_time[num] += float(curline.split('\t')[6])                       
                        multihap_cpu_rows[num] += int(curline.split('\t')[7])  
                        multihap_fpga_rows[num] += int(curline.split('\t')[8])                            
                        if multihap_sched_type[num] == '':
                            multihap_sched_type[num] = curline.split('\t')[9]    
                        multihap_cpu_power[num] += float(curline.split('\t')[10])
                        multihap_fpga_power[num] += float(curline.split('\t')[11])
                        multihap_cpu_energy[num] += float(curline.split('\t')[12])
                        multihap_fpga_energy[num] += float(curline.split('\t')[13])

                multihap_chunk_size[num] /= multihap_runs[num] 
                multihap_exec_time[num] /= multihap_runs[num]
                multihap_cpu_power[num] /= multihap_runs[num]
                multihap_fpga_power[num] /= multihap_runs[num]
                multihap_cpu_energy[num] /= multihap_runs[num]
                multihap_fpga_energy[num] /= multihap_runs[num]
                if multihap_cpu_rows[num] != 0:
                    multihap_cpu_rows[num] /= multihap_runs[num]
                if multihap_fpga_rows[num] != 0:
                    multihap_fpga_rows[num] /= multihap_runs[num]
                                                                 
                multihap_aggregated_results.append(str(multihap_runs[num])+'\t'+str(multihap_cpu_cores[num])+'\t'+str(multihap_fpga_hpacc[num])+'\t'+str(multihap_fpga_hpcacc[num])+'\t'+str(multihap_chunk_size[num])+'\t'+multihap_ioctl_flag[num]+'\t'+str(multihap_exec_time[num])+'\t'+str(multihap_cpu_rows[num])+'\t'+str(multihap_fpga_rows[num])+'\t'+multihap_sched_type[num]+'\t'+str(multihap_cpu_power[num])+'\t'+str(multihap_fpga_power[num])+'\t'+str(multihap_cpu_energy[num])+'\t'+str(multihap_fpga_energy[num]))
            
    #Data Output
    print 'Aggregated Input Data:'
    for num, infile in enumerate(inputfile): 
        print "File " + str(num+1) + ": " + infile
        print inputfile_header + aggregated_results[num] + '\n'
    
    if ideal_multiplier > 0:
        print 'Aggregated Ideal Input Data:'
        for num, infile in enumerate(ideal_inputfile): 
            print "File " + str(num+1) + ": " + infile
            print inputfile_header + ideal_aggregated_results[num] + '\n'
    
    if multihap_inputfile != []:
        print 'Aggregated MultiHap Input Data:'
        for num, infile in enumerate(multihap_inputfile): 
            print "File " + str(num+1) + ": " + infile
            print inputfile_header + multihap_aggregated_results[num] + '\n'         
        
    #Plotting part of the script 
    if plottype == 1:   
        #Process aggregated data for plot type 1
        #This means converting the data from per file to per configuration type
        configs = []
        conf_exec_time = []
        conf_chunk_size = []
        for num, infile in enumerate(inputfile):
            #Check if new configuration
            temp_conf = sched_type[num]+"+IOCTL(" + ioctl_flag[num] + "):"+str(cpu_cores[num])+"C+"+str(fpga_hpacc[num])+"HP+"+str(fpga_hpcacc[num])+"HPC"
            if temp_conf not in configs:
                configs.append(temp_conf)
                conf_exec_time.append([])
                conf_exec_time[-1].append(exec_time[num]) #add value to last element in array
                conf_chunk_size.append([])
                conf_chunk_size[-1].append(chunk_size[num])
            #If configuration already found just add contents     
            else:
                temp_ind=configs.index(temp_conf) #get index of entry
                conf_exec_time[temp_ind].append(exec_time[num])
                conf_chunk_size[temp_ind].append(chunk_size[num])                               
         
        if ideal_multiplier > 0:
             for num, infile in enumerate(ideal_inputfile):
                  #Check if new configuration
                  temp_conf = ideal_sched_type[num]+"(IDEAL):"+str(ideal_cpu_cores[num])+"C+"+str(ideal_fpga_hpacc[num])+"HP+"+str(ideal_fpga_hpcacc[num])+"HPC"
                  if temp_conf not in configs:
                       configs.append(temp_conf)
                       conf_exec_time.append([])
                       conf_exec_time[-1].append(ideal_exec_time[num]) #add value to last element in array
                       conf_chunk_size.append([])
                       conf_chunk_size[-1].append(ideal_chunk_size[num])
                  #If configuration already found just add contents     
                  else:
                       temp_ind=configs.index(temp_conf) #get index of entry
                       conf_exec_time[temp_ind].append(ideal_exec_time[num])
                       conf_chunk_size[temp_ind].append(ideal_chunk_size[num])
                                               
        if multihap_inputfile != []:
            for num, infile in enumerate(multihap_inputfile):
                  #Check if new configuration
                  temp_conf = multihap_sched_type[num]+"+IOCTL(" + multihap_ioctl_flag[num] + "):"+str(multihap_cpu_cores[num])+"C+"+str(multihap_fpga_hpacc[num])+"HP+"+str(multihap_fpga_hpcacc[num])+"HPC"
                  if temp_conf not in configs:
                       configs.append(temp_conf)
                       conf_exec_time.append([])
                       conf_exec_time[-1].append(multihap_exec_time[num]) #add value to last element in array
                       conf_chunk_size.append([])
                       conf_chunk_size[-1].append(multihap_chunk_size[num])
                  #If configuration already found just add contents     
                  else:
                       temp_ind=configs.index(temp_conf) #get index of entry
                       conf_exec_time[temp_ind].append(multihap_exec_time[num])
                       conf_chunk_size[temp_ind].append(multihap_chunk_size[num])                          
                
        #Sanity check that we have collected all configs
        print 'Collected configs:'
        for num, conf in enumerate(configs): 
            print "Configuration " + str(num+1) + ": " + conf       
            print "Exec times: " + str(conf_exec_time[num])
            print "Chunk sizes: " + str(conf_chunk_size[num]) + '\n'          
    
        fig, ax = plt.subplots()
        
        color=iter(cm.rainbow(np.linspace(0,1,num+1)))
        
        for num, conf in enumerate(configs):
            temparr = []
            for i in range(len(conf_chunk_size[num])):
                temparr.append([conf_chunk_size[num][i],conf_exec_time[num][i]])
            temparr = sorted(temparr, key=itemgetter(0))
            marker_y = min([i[1] for i in temparr])
            marker_index = [i[1] for i in temparr].index(marker_y)
            marker_x = [i[0] for i in temparr][marker_index]
            #templine = ax.plot([i[0] for i in temparr],[i[1] for i in temparr], label=configs[num])
            c=next(color)
            templine = mlines.Line2D([i[0] for i in temparr],[i[1] for i in temparr], label=configs[num], color = c)
            ax.add_line(templine)
            ax.plot(marker_x, marker_y, marker='*', markersize='10', alpha=1, color=templine.get_color())
        
        bins_np = np.array(range(min(map(min, conf_chunk_size)),max(map(max, conf_chunk_size))+250,250)) 
        plt.xticks(np.arange(min(bins_np), max(bins_np) + (bins_np[1] - bins_np[0]), bins_np[1] - bins_np[0]))
        #title='About as simple as it gets, folks')
        ax.set(xlabel='Chunk Size [#]', ylabel='Execution Time [ms]')
       
        custom_line = mlines.Line2D([], [], color='black', marker='*', markersize='10', label='Fastest Point Marker')
        axhandles, dud = ax.get_legend_handles_labels()
        axhandles.append(custom_line)
        ax.legend(handles=axhandles,loc=0)        
        ax.grid()

        fig.tight_layout()
        plt.show()
        
        if outputfile != '':
            pp = PdfPages(outputfile)
            pp.savefig(fig) 
            pp.close()
 
    if plottype == 2:   
        #Process aggregated data for plot type 2
        configs = []
        conf_exec_time = []
        conf_cpu_throughput = []
        conf_fpga_throughput = []
        conf_chunk_size = []

        for num, infile in enumerate(inputfile):
            #Check if new configuration
            temp_conf = sched_type[num]+"+IOCTL(" + ioctl_flag[num] + "):"+str(cpu_cores[num])+"C+"+str(fpga_hpacc[num])+"HP+"+str(fpga_hpcacc[num])+"HPC"
            if temp_conf not in configs:
                configs.append(temp_conf)
                conf_exec_time.append([])
                conf_exec_time[-1].append(exec_time[num]) #add value to last element in array
                conf_cpu_throughput.append([])
                conf_cpu_throughput[-1].append(cpu_rows[num]/exec_time[num])           
                conf_fpga_throughput.append([])
                conf_fpga_throughput[-1].append(fpga_rows[num]/exec_time[num])               
                conf_chunk_size.append([])
                conf_chunk_size[-1].append(chunk_size[num])
            #If configuration already found just add contents     
            else:
                temp_ind=configs.index(temp_conf) #get index of entry
                conf_exec_time[temp_ind].append(exec_time[num])
                conf_cpu_throughput[temp_ind].append(cpu_rows[num]/exec_time[num])
                conf_fpga_throughput[temp_ind].append(fpga_rows[num]/exec_time[num])
                conf_chunk_size[temp_ind].append(chunk_size[num])
                
        if ideal_multiplier > 0:
            for num, infile in enumerate(ideal_inputfile):
                #Check if new configuration
                temp_conf = sched_type[num]+"(IDEAL):"+str(ideal_cpu_cores[num])+"C+"+str(ideal_fpga_hpacc[num])+"HP+"+str(ideal_fpga_hpcacc[num])+"HPC"
                if temp_conf not in configs:
                    configs.append(temp_conf)
                    conf_exec_time.append([])
                    conf_exec_time[-1].append(ideal_exec_time[num]) #add value to last element in array
                    conf_cpu_throughput.append([])
                    conf_cpu_throughput[-1].append(ideal_cpu_rows[num]/ideal_exec_time[num])           
                    conf_fpga_throughput.append([])
                    conf_fpga_throughput[-1].append(ideal_fpga_rows[num]/ideal_exec_time[num])               
                    conf_chunk_size.append([])
                    conf_chunk_size[-1].append(ideal_chunk_size[num])
                #If configuration already found just add contents     
                else:
                    temp_ind=configs.index(temp_conf) #get index of entry
                    conf_exec_time[temp_ind].append(ideal_exec_time[num])
                    conf_cpu_throughput[temp_ind].append(ideal_cpu_rows[num]/ideal_exec_time[num])
                    conf_fpga_throughput[temp_ind].append(ideal_fpga_rows[num]/ideal_exec_time[num])
                    conf_chunk_size[temp_ind].append(ideal_chunk_size[num])  

                
        #Sanity check that we have collected all configs
        print 'Collected configs:'
        for num, conf in enumerate(configs): 
            print "Configuration " + str(num+1) + ": " + conf       
            print "Exec times: " + str(conf_exec_time[num])
            print "CPU Throughput: " + str(conf_cpu_throughput[num])
            print "FPGA Throughput: " + str(conf_fpga_throughput[num])
            print "Chunk sizes: " + str(conf_chunk_size[num]) + '\n'          
    
        fig, ax1 = plt.subplots()
        #ax2 = ax1.twinx()
        
        rects = []
        opacity_front = 0.95
        opacity_back = 0.35  
        
        color=iter(cm.rainbow(np.linspace(0,1,num+1)))   
        
        for num, conf in enumerate(configs):
            temparr = []
            for i in range(len(conf_chunk_size[num])):
                temparr.append([conf_chunk_size[num][i],conf_cpu_throughput[num][i],conf_fpga_throughput[num][i]])
            temparr = sorted(temparr, key=itemgetter(0))
            bins_np = np.array([i[0] for i in temparr]) 
            bar_width = ((bins_np[1] - bins_np[0])/len(configs))*0.75
            bar_offset = (bar_width*len(configs))/2
            c=next(color)
            if ideal_multiplier > 0 and num == len(configs)-1:
                templine = mlines.Line2D([i[0] for i in temparr],[i[1] for i in temparr]+[i[2] for i in temparr], label=configs[num], color = c)
                ax1.add_line(templine)
            else:
                bar = ax1.bar(bins_np-bar_offset+num*bar_width,[i[1] for i in temparr], bar_width, alpha = opacity_front, edgecolor='black', color=c, align = 'edge', label=configs[num])
                rects.append(bar)
                #rects.append(ax1.bar(bins_np-bar_offset+num*bar_width,[i[2] for i in temparr], bar_width, alpha = opacity_back, edgecolor='black', align = 'edge', label='_nolegend_', color=bar.patches[0].get_facecolor()))
                ax1.errorbar(bins_np-bar_offset+num*bar_width+bar_width/2, [i[1] for i in temparr], [i[2] for i in temparr], lolims='False', fmt='none' ,ecolor=bar.patches[0].get_facecolor())
                #ax2.bar(bins_np-bar_offset+num*bar_width,[i[1] for i in temparr], bar_width, align = 'edge', alpha = 0, label='_nolegend_', hatch = '/', color='None')
                #ax2.bar(bins_np-bar_offset+num*bar_width,[i[2] for i in temparr], bar_width, align = 'edge', alpha = 0, label='_nolegend_', hatch = '\\', color='None')
            

        xcoords = bins_np+(bins_np[1]-bins_np[0])/2
        for xc in xcoords:
            plt.axvline(x=xc, color='black')
        
        ax1.set(xlabel='Chunk Size [#]', ylabel='Throughput [rows/ms]')
        #ax2.set_yticks([])
        #ax2.set_yticklabels([])
        
        plt.xticks(np.arange(min(bins_np), max(bins_np) + (bins_np[1] - bins_np[0]), bins_np[1] - bins_np[0]))
        
        ax1.grid(which='major', axis='y', linestyle='-')
        
        #l = ax1.get_ylim()
        #l2 = ax2.get_ylim()
        #f = lambda x : l2[0]+(x-l[0])/(l[1]-l[0])*(l2[1]-l2[0])
        #ticks = f(ax1.get_yticks())
        #ax2.yaxis.set_major_locator(matplotlib.ticker.FixedLocator(ticks))
       
        custom_hatch = mpatches.Rectangle((0,0), 0, 0, edgecolor='black', facecolor='white', alpha = opacity_front, label='CPU')
        custom_arrow = mlines.Line2D([], [], color='black', label='FPGA')
        ax1handles, dud = ax1.get_legend_handles_labels()
        ax1handles.append(custom_hatch)
        ax1handles.append(custom_arrow)
        #ax1.legend(handles=ax1handles,loc=0)
        ax1.legend(handles=ax1handles,loc=0,handler_map={mlines.Line2D : ArrowHandler()})

        fig.tight_layout()
        plt.show()
        
        if outputfile != '':
            pp = PdfPages(outputfile)
            pp.savefig(fig) 
            pp.close()                      
                
    if plottype == 3:
        #Power/Energy Bar chart
        config_labels = []
        for num, infile in enumerate(inputfile):
            config_labels.append(sched_type[num]+"+IOCTL(" + ioctl_flag[num] + "):"+str(cpu_cores[num])+"C+"+str(fpga_hpacc[num])+"HP+"+str(fpga_hpcacc[num])+"HPC")
                
        n_groups = len(inputfile)
        cpu_power_np = np.array(cpu_power)
        fpga_power_np = np.array(fpga_power)
        cpu_energy_np = np.array(cpu_energy)
        fpga_energy_np = np.array(fpga_energy)        
            
        rects = []
        
        fig, ax1 = plt.subplots()

        index = np.arange(n_groups) 
        color=iter(cm.rainbow(np.linspace(0,1,num+1)))   
        bar_width = 0.15
        opacity_front = 1
        opacity_back = 1
        c=next(color)
        tempbar = ax1.bar(index+bar_width, cpu_power_np, bar_width, alpha=opacity_front, color=c, edgecolor='black', label='Power')
        ax1.errorbar(index+bar_width, cpu_power_np, fpga_power_np, lolims='False', fmt='none' ,ecolor=tempbar.patches[0].get_facecolor())
        
        ax1.set_xlabel('Configuration')
        ax1.set_ylabel('Power [W]')
        ax1.set_xticks(index + bar_width*3 / 2)
        
        ax1.set_xticklabels(config_labels,fontsize='small',rotation='45')
        
        ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
        c=next(color)        
        tempbar = ax2.bar(index + bar_width*2, cpu_energy_np, bar_width, alpha=opacity_back, color=c, edgecolor='black', label='Energy')
        ax2.errorbar(index+bar_width*2, cpu_energy_np, fpga_energy_np, lolims='False', fmt='none' ,ecolor=tempbar.patches[0].get_facecolor())
        ax2.set_ylabel('Energy [J]')  # we already handled the x-label with ax1
        
        #Construct the legend
        custom_hatch = mpatches.Rectangle((0,0), 0, 0, edgecolor='black', facecolor='white', alpha = opacity_front, label='CPU')
        custom_arrow = mlines.Line2D([], [], color='black', label='FPGA')
        ax1handles, dud = ax1.get_legend_handles_labels()
        ax1handles.append(tempbar)
        ax1handles.append(custom_hatch)
        ax1handles.append(custom_arrow)
        ax1.legend(handles=ax1handles,loc=0,handler_map={mlines.Line2D : ArrowHandler()})

        fig.tight_layout()
        plt.show()
        
        if outputfile != '':
            pp = PdfPages(outputfile)
            pp.savefig(fig) 
            pp.close() 
            
if __name__ == "__main__":
   main(sys.argv[1:])



