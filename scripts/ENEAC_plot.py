
##############################################################################
# Adapted from: Antonio Vilches
# Revised by:   Kris Nikov
# Version:      2.0
# Date:         18/04/2019
# Description:  Plotting script for ENEAC
# Copyright:    University of Bristol
##############################################################################

#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from matplotlib.ticker import ScalarFormatter
from matplotlib.ticker import MaxNLocator
from collections import namedtuple
import matplotlib.patches as mpatches
from operator import itemgetter
import os



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
    fpga_work = []
    sched_type = []
    total_power = []
    fpga_power = []
    total_energy = []
    fpga_energy = []
    aggregated_results = []
    
    try:
        opts, args = getopt.getopt(argv,"hp:i:d:o:",["ptype=","ifile=","idir=","ofile="])
    except getopt.GetoptError:
        print 'Error: use -h option to see usage!'
        sys.exit(2)
    
    for opt, arg in opts:
        if opt == '-h':
            print 'Usage: -p <plot type> -i <input file> -d <input directory> -o <output file>'
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
                    if fileopentest.readline() != 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tWork Offloaded on FPGA[%]\tScheduler Type\tTotal Power[W]\tFPGA Power[W]\tTotal Energy[J]\tFPGA Energy[J]\n':
                        print 'Error in option <-i ' + arg + '>: file <' + arg + '> is not the correct format or is empty. Please enter a valid input file.'
                        print 'The results file should start with the following header, followed by the data:'
                        print 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tWork Offloaded on FPGA[%]\tScheduler Type\tTotal Power[W]\tFPGA Power[W]\tTotal Energy[J]\tFPGA Energy[J]'
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
                                if fileopentest.readline() != 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tWork Offloaded on FPGA[%]\tScheduler Type\tTotal Power[W]\tFPGA Power[W]\tTotal Energy[J]\tFPGA Energy[J]\n':
                                    print 'Error in option <-d ' + arg + '>: directory <' + arg + '> contains illegal input files (' + tempinputfile + '). Please enter a valid input directory.'
                                    print 'The results file should start with the following header, followed by the data:'
                                    print 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tWork Offloaded on FPGA[%]\tScheduler Type\tTotal Power[W]\tFPGA Power[W]\tTotal Energy[J]\tFPGA Energy[J]'
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
    
    print 'Plot type is ' + str(plottype)
    print 'Input file is ' + str(inputfile)[1:-1]
    print 'Output file is ' + outputfile
    
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
            fpga_work.append(-1)
            sched_type.append('')
            total_power.append(0)
            fpga_power.append(0)
            total_energy.append(0)
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
                    if fpga_work[num] == -1:
                        fpga_work[num] = int(curline.split('\t')[7])       
                    if sched_type[num] == '':
                        sched_type[num] = curline.split('\t')[8]    
                    total_power[num] += float(curline.split('\t')[9])
                    fpga_power[num] += float(curline.split('\t')[10])
                    total_energy[num] += float(curline.split('\t')[11])
                    fpga_energy[num] += float(curline.split('\t')[12])

            exec_time[num] /= runs[num]
            total_power[num] /= runs[num]
            fpga_power[num] /= runs[num]
            total_energy[num] /= runs[num]
            fpga_energy[num] /= runs[num]           
            aggregated_results.append(str(runs[num])+'\t'+str(cpu_cores[num])+'\t'+str(fpga_hpacc[num])+'\t'+str(fpga_hpcacc[num])+'\t'+str(chunk_size[num])+'\t'+ioctl_flag[num]+'\t'+str(exec_time[num])+'\t'+str(fpga_work[num])+'\t'+sched_type[num]+'\t'+str(total_power[num])+'\t'+str(fpga_power[num])+'\t'+str(total_energy[num])+'\t'+str(fpga_energy[num]))     
            
            
    #Data Output
    print 'Aggregated Input Data:'
    for num, infile in enumerate(inputfile): 
        print "File " + str(num+1) + ": " + infile       
        print inputfile_header + aggregated_results[num] + '\n'
        
    #Plotting part of the script
    
    if plottype == 1:   
        #Process aggregated data for plot type 1
        #This means converting the data from per file to per configuration type
        configs = []
        conf_exec_time = []
        conf_chunk_size = []
        for num, infile in enumerate(inputfile):
            #Check if new configuration
            temp_conf = sched_type[num]+":"+str(cpu_cores[num])+"C+"+str(fpga_hpacc[num])+"HP+"+str(fpga_hpcacc[num])+"HPC"
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
                
        #Sanity check that we have collected all configs
        print 'Collected configs:'
        for num, conf in enumerate(configs): 
            print "Configuration " + str(num+1) + ": " + conf       
            print "Exec times: " + str(conf_exec_time[num])
            print "Chunk sizes: " + str(conf_chunk_size[num]) + '\n'          
    
        fig, ax = plt.subplots()
        
        for num, conf in enumerate(configs):
            temparr = []
            for i in range(len(conf_chunk_size[num])):
                temparr.append([conf_chunk_size[num][i],conf_exec_time[num][i]])
            print str(temparr)    
            temparr = sorted(temparr, key=itemgetter(0))
            print [i[0] for i in temparr]
            print [i[1] for i in temparr]
            ax.plot([i[0] for i in temparr],[i[1] for i in temparr], label=configs[num])
        
        
        ax.set(xlabel='Chunk Size [#]', ylabel='Execution Time [ms]')
        #title='About as simple as it gets, folks')
       
        ax.grid()
        ax.legend()

        fig.tight_layout()
        plt.show()
        
        if outputfile != '':
            pp = PdfPages(outputfile)
            pp.savefig(fig) 
            pp.close()
    
    if plottype == 2:
        #Bar chart
        n_groups = len(inputfile)
        rects = []

        fig, ax1 = plt.subplots()

        index = np.arange(n_groups) + np.arange(n_groups) 
        bar_width = 0.35
        opacity = 0.35

        rects.append(ax1.bar(index, total_power, bar_width, alpha=opacity, color='b',label='Total Power'))
        rects.append(ax1.bar(index + bar_width, fpga_power, bar_width, alpha=opacity, color='c', label='FPGA Power'))

        ax1.set_xlabel('Configuration')
        ax1.set_ylabel('Power [W]')
        #ax1.set_title('')
        ax1.set_xticks(index + bar_width*3 / 2)
        
        config_labels = []
        for num, infile in enumerate(inputfile):
            config_labels.append(sched_type[num]+":"+str(cpu_cores[num])+"C+"+str(fpga_hpacc[num])+"HP+"+str(fpga_hpcacc[num])+"HPC")
        
        ax1.set_xticklabels(config_labels,fontsize='small',rotation='90')
        
        ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
        rects.append(ax2.bar(index + bar_width*2, total_energy, bar_width, alpha=opacity, color='r',label='Total Energy'))
        rects.append(ax2.bar(index + bar_width*3, fpga_energy, bar_width, alpha=opacity, color='m', label='FPGA Energy'))
        ax2.set_ylabel('Energy [J]')  # we already handled the x-label with ax1
        
        #Construct the legend
        labs = [r.get_label() for r in rects]
        ax1.legend(rects, labs, loc=0)

        fig.tight_layout()
        plt.show()
        
        if outputfile != '':
            pp = PdfPages(outputfile)
            pp.savefig(fig) 
            pp.close()
    
    
if __name__ == "__main__":
   main(sys.argv[1:])



