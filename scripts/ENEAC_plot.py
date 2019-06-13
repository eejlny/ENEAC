
##############################################################################
# Adapted from: Antonio Vilches
# Revised by:   Kris Nikov
# Version:      3.0
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
import copy
import matplotlib.ticker
import matplotlib.lines as mlines



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
                    if fileopentest.readline() != 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tTotal Power[W]\tFPGA Power[W]\tTotal Energy[J]\tFPGA Energy[J]\n':
                        print 'Error in option <-i ' + arg + '>: file <' + arg + '> is not the correct format or is empty. Please enter a valid input file.'
                        print 'The results file should start with the following header, followed by the data:'
                        print 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tTotal Power[W]\tFPGA Power[W]\tTotal Energy[J]\tFPGA Energy[J]'
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
                                if fileopentest.readline() != 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tTotal Power[W]\tFPGA Power[W]\tTotal Energy[J]\tFPGA Energy[J]\n':
                                    print 'Error in option <-d ' + arg + '>: directory <' + arg + '> contains illegal input files (' + tempinputfile + '). Please enter a valid input directory.'
                                    print 'The results file should start with the following header, followed by the data:'
                                    print 'Run[#]\tCPU Cores[#]\tFPGA HP Accelerators[#]\tFPGA HPC Accelerators[#]\tChunk Size[#]\tIOCTL Enabled\tExecution Time[ms]\tRows on CPU[#]\tRows on FPGA[#]\tScheduler Type\tTotal Power[W]\tFPGA Power[W]\tTotal Energy[J]\tFPGA Energy[J]'
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
            cpu_rows.append(0)
            fpga_rows.append(0)
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
                    cpu_rows[num] += int(curline.split('\t')[7])  
                    fpga_rows[num] += int(curline.split('\t')[8])                            
                    if sched_type[num] == '':
                        sched_type[num] = curline.split('\t')[9]    
                    total_power[num] += float(curline.split('\t')[10])
                    fpga_power[num] += float(curline.split('\t')[11])
                    total_energy[num] += float(curline.split('\t')[12])
                    fpga_energy[num] += float(curline.split('\t')[13])

            exec_time[num] /= runs[num]
            total_power[num] /= runs[num]
            fpga_power[num] /= runs[num]
            total_energy[num] /= runs[num]
            fpga_energy[num] /= runs[num]
            if cpu_rows[num] != 0:
                cpu_rows[num] /= runs[num]
            if fpga_rows[num] != 0:
                fpga_rows[num] /= runs[num]    
                     
            aggregated_results.append(str(runs[num])+'\t'+str(cpu_cores[num])+'\t'+str(fpga_hpacc[num])+'\t'+str(fpga_hpcacc[num])+'\t'+str(chunk_size[num])+'\t'+ioctl_flag[num]+'\t'+str(exec_time[num])+'\t'+str(cpu_rows[num])+'\t'+str(fpga_rows[num])+'\t'+sched_type[num]+'\t'+str(total_power[num])+'\t'+str(fpga_power[num])+'\t'+str(total_energy[num])+'\t'+str(fpga_energy[num]))     
            
            
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
            temparr = sorted(temparr, key=itemgetter(0))
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
                
        #Sanity check that we have collected all configs
        print 'Collected configs:'
        for num, conf in enumerate(configs): 
            print "Configuration " + str(num+1) + ": " + conf       
            print "Exec times: " + str(conf_exec_time[num])
            print "CPU Throughput: " + str(conf_cpu_throughput[num])
            print "FPGA Throughput: " + str(conf_fpga_throughput[num])
            print "Chunk sizes: " + str(conf_chunk_size[num]) + '\n'          
    
        fig, ax1 = plt.subplots()
        ax2 = ax1.twinx()
        
        rects = []
        opacity_front = 0.95
        opacity_back = 0.5        
        
        for num, conf in enumerate(configs):
            temparr = []
            for i in range(len(conf_chunk_size[num])):
                temparr.append([conf_chunk_size[num][i],conf_cpu_throughput[num][i],conf_fpga_throughput[num][i]])
            temparr = sorted(temparr, key=itemgetter(0))
            bins_np = np.array([i[0] for i in temparr]) 
            bar_width = ((bins_np[1] - bins_np[0])/len(configs))*0.90
            bar_offset = (bar_width*len(configs))/2
            bar = ax1.bar(bins_np-bar_offset+num*bar_width,[i[1] for i in temparr], bar_width, align = 'edge', alpha = opacity_front, label=configs[num])
            rects.append(bar)
            rects.append(ax1.bar(bins_np-bar_offset+num*bar_width,[i[2] for i in temparr], bar_width, align = 'edge', alpha = opacity_back, label='_nolegend_', color=bar.patches[0].get_facecolor()))
            ax2.bar(bins_np-bar_offset+num*bar_width,[i[1] for i in temparr], bar_width, align = 'edge', alpha = 0, label='_nolegend_', hatch = '/', color='None')
            ax2.bar(bins_np-bar_offset+num*bar_width,[i[2] for i in temparr], bar_width, align = 'edge', alpha = 0, label='_nolegend_', hatch = '\\', color='None')

        
        ax1.set(xlabel='Chunk Size [#]', ylabel='Throughput [rows/ms]')
        ax2.set_yticks([])
        ax2.set_yticklabels([])
        
        plt.xticks(np.arange(min(bins_np), max(bins_np) + (bins_np[1] - bins_np[0]), bins_np[1] - bins_np[0]))
        
        ax1.grid(which='major', axis='y', linestyle='-')
        
        l = ax1.get_ylim()
        l2 = ax2.get_ylim()
        f = lambda x : l2[0]+(x-l[0])/(l[1]-l[0])*(l2[1]-l2[0])
        ticks = f(ax1.get_yticks())
        ax2.yaxis.set_major_locator(matplotlib.ticker.FixedLocator(ticks))
       
        custom_hatch1 = mpatches.Rectangle((0,0), 0, 0, edgecolor='black', facecolor='None', hatch = '/', label='CPU')
        custom_hatch2 = mpatches.Rectangle((0,0), 0, 0, edgecolor='black', facecolor='None', hatch = '\\', label='FPGA')
        ax1handles, dud = ax1.get_legend_handles_labels()
        ax1handles.append(custom_hatch1)
        ax1handles.append(custom_hatch2)
        ax2.legend(handles=ax1handles,loc=0)

        fig.tight_layout()
        plt.show()
        
        if outputfile != '':
            pp = PdfPages(outputfile)
            pp.savefig(fig) 
            pp.close()

    if plottype == 3:
        #Process aggregated data for plot type 3 - chunksize power graph
        configs = []
        conf_fpga_power = []
        conf_total_power = []
        conf_chunk_size = []

        for num, infile in enumerate(inputfile):
            #Check if new configuration
            temp_conf = sched_type[num]+"+IOCTL(" + ioctl_flag[num] + "):"+str(cpu_cores[num])+"C+"+str(fpga_hpacc[num])+"HP+"+str(fpga_hpcacc[num])+"HPC"
            if temp_conf not in configs:
                configs.append(temp_conf)
                conf_fpga_power.append([])
                conf_fpga_power[-1].append(fpga_power[num]) #add value to last element in array
                conf_total_power.append([])
                conf_total_power[-1].append(total_power[num])
                conf_chunk_size.append([])
                conf_chunk_size[-1].append(chunk_size[num])
            #If configuration already found just add contents     
            else:
                temp_ind=configs.index(temp_conf) #get index of entry
                conf_fpga_power[temp_ind].append(fpga_power[num])
                conf_total_power[temp_ind].append(total_power[num])
                conf_chunk_size[temp_ind].append(chunk_size[num]) 
                
        #Sanity check that we have collected all configs
        print 'Collected configs:'
        for num, conf in enumerate(configs): 
            print "Configuration " + str(num+1) + ": " + conf       
            print "FPGA Power: " + str(conf_fpga_power[num])
            print "Total Power: " + str(conf_total_power[num])
            print "Chunk sizes: " + str(conf_chunk_size[num]) + '\n'          
    
        fig, ax1 = plt.subplots()
        ax2 = ax1.twinx()
        
        rects = []
        opacity_front = 0.95
        opacity_back = 0.5        
        
        for num, conf in enumerate(configs):
            temparr = []
            for i in range(len(conf_chunk_size[num])):
                temparr.append([conf_chunk_size[num][i],conf_fpga_power[num][i],conf_total_power[num][i]])
            temparr = sorted(temparr, key=itemgetter(0))
            bins_np = np.array([i[0] for i in temparr]) 
            bar_width = ((bins_np[1] - bins_np[0])/len(configs))*0.90
            bar_offset = (bar_width*len(configs))/2
            bar = ax1.bar(bins_np-bar_offset+num*bar_width,[i[1] for i in temparr], bar_width, align = 'edge', alpha = opacity_front, label=configs[num])
            rects.append(bar)
            rects.append(ax1.bar(bins_np-bar_offset+num*bar_width,[i[2] for i in temparr], bar_width, align = 'edge', alpha = opacity_back, label='_nolegend_', color=bar.patches[0].get_facecolor()))
            ax2.bar(bins_np-bar_offset+num*bar_width,[i[1] for i in temparr], bar_width, align = 'edge', alpha = 0, label='_nolegend_', hatch = '/', color='None')
            ax2.bar(bins_np-bar_offset+num*bar_width,[i[2] for i in temparr], bar_width, align = 'edge', alpha = 0, label='_nolegend_', hatch = '\\', color='None')

        
        ax1.set(xlabel='Chunk Size [#]', ylabel='Power [W]')
        ax2.set_yticks([])
        ax2.set_yticklabels([])
        
        plt.xticks(np.arange(min(bins_np), max(bins_np) + (bins_np[1] - bins_np[0]), bins_np[1] - bins_np[0]))
        
        ax1.grid(which='major', axis='y', linestyle='-')
        
        l = ax1.get_ylim()
        l2 = ax2.get_ylim()
        f = lambda x : l2[0]+(x-l[0])/(l[1]-l[0])*(l2[1]-l2[0])
        ticks = f(ax1.get_yticks())
        ax2.yaxis.set_major_locator(matplotlib.ticker.FixedLocator(ticks))
       
        custom_hatch1 = mpatches.Rectangle((0,0), 0, 0, edgecolor='black', facecolor='None', hatch = '/', label='FPGA')
        custom_hatch2 = mpatches.Rectangle((0,0), 0, 0, edgecolor='black', facecolor='None', hatch = '\\', label='Total')
        ax1handles, dud = ax1.get_legend_handles_labels()
        ax1handles.append(custom_hatch1)
        ax1handles.append(custom_hatch2)
        ax2.legend(handles=ax1handles,loc=0)

        fig.tight_layout()
        plt.show()
        
        if outputfile != '':
            pp = PdfPages(outputfile)
            pp.savefig(fig) 
            pp.close() 
            
    if plottype == 4:
        #Process aggregated data for plot type 3 - chunksize energy graph
        configs = []
        conf_fpga_energy = []
        conf_total_energy = []
        conf_chunk_size = []

        for num, infile in enumerate(inputfile):
            #Check if new configuration
            temp_conf = sched_type[num]+"+IOCTL(" + ioctl_flag[num] + "):"+str(cpu_cores[num])+"C+"+str(fpga_hpacc[num])+"HP+"+str(fpga_hpcacc[num])+"HPC"
            if temp_conf not in configs:
                configs.append(temp_conf)
                conf_fpga_energy.append([])
                conf_fpga_energy[-1].append(fpga_energy[num]) #add value to last element in array
                conf_total_energy.append([])
                conf_total_energy[-1].append(total_energy[num])
                conf_chunk_size.append([])
                conf_chunk_size[-1].append(chunk_size[num])
            #If configuration already found just add contents     
            else:
                temp_ind=configs.index(temp_conf) #get index of entry
                conf_fpga_energy[temp_ind].append(fpga_energy[num])
                conf_total_energy[temp_ind].append(total_energy[num])
                conf_chunk_size[temp_ind].append(chunk_size[num]) 
                
        #Sanity check that we have collected all configs
        print 'Collected configs:'
        for num, conf in enumerate(configs): 
            print "Configuration " + str(num+1) + ": " + conf       
            print "FPGA Power: " + str(conf_fpga_energy[num])
            print "Total Power: " + str(conf_total_energy[num])
            print "Chunk sizes: " + str(conf_chunk_size[num]) + '\n'          
    
        fig, ax1 = plt.subplots()
        ax2 = ax1.twinx()
        
        rects = []
        opacity_front = 0.95
        opacity_back = 0.5        
        
        for num, conf in enumerate(configs):
            temparr = []
            for i in range(len(conf_chunk_size[num])):
                temparr.append([conf_chunk_size[num][i],conf_fpga_energy[num][i],conf_total_energy[num][i]])
            temparr = sorted(temparr, key=itemgetter(0))
            bins_np = np.array([i[0] for i in temparr]) 
            bar_width = ((bins_np[1] - bins_np[0])/len(configs))*0.90
            bar_offset = (bar_width*len(configs))/2
            bar = ax1.bar(bins_np-bar_offset+num*bar_width,[i[1] for i in temparr], bar_width, align = 'edge', alpha = opacity_front, label=configs[num])
            rects.append(bar)
            rects.append(ax1.bar(bins_np-bar_offset+num*bar_width,[i[2] for i in temparr], bar_width, align = 'edge', alpha = opacity_back, label='_nolegend_', color=bar.patches[0].get_facecolor()))
            ax2.bar(bins_np-bar_offset+num*bar_width,[i[1] for i in temparr], bar_width, align = 'edge', alpha = 0, label='_nolegend_', hatch = '/', color='None')
            ax2.bar(bins_np-bar_offset+num*bar_width,[i[2] for i in temparr], bar_width, align = 'edge', alpha = 0, label='_nolegend_', hatch = '\\', color='None')

        
        ax1.set(xlabel='Chunk Size [#]', ylabel='Energy [J]')
        ax2.set_yticks([])
        ax2.set_yticklabels([])
        
        plt.xticks(np.arange(min(bins_np), max(bins_np) + (bins_np[1] - bins_np[0]), bins_np[1] - bins_np[0]))
        
        ax1.grid(which='major', axis='y', linestyle='-')
        
        l = ax1.get_ylim()
        l2 = ax2.get_ylim()
        f = lambda x : l2[0]+(x-l[0])/(l[1]-l[0])*(l2[1]-l2[0])
        ticks = f(ax1.get_yticks())
        ax2.yaxis.set_major_locator(matplotlib.ticker.FixedLocator(ticks))
       
        custom_hatch1 = mpatches.Rectangle((0,0), 0, 0, edgecolor='black', facecolor='None', hatch = '/', label='FPGA')
        custom_hatch2 = mpatches.Rectangle((0,0), 0, 0, edgecolor='black', facecolor='None', hatch = '\\', label='Total')
        ax1handles, dud = ax1.get_legend_handles_labels()
        ax1handles.append(custom_hatch1)
        ax1handles.append(custom_hatch2)
        ax2.legend(handles=ax1handles,loc=0)

        fig.tight_layout()
        plt.show()
        
        if outputfile != '':
            pp = PdfPages(outputfile)
            pp.savefig(fig) 
            pp.close()                         
                
    if plottype == 5:
        #Bar chart
        n_groups = len(inputfile)
        rects = []
        
        total_power_np = np.array(total_power)
        fpga_power_np = np.array(fpga_power)
        total_energy_np = np.array(total_energy)
        fpga_energy_np = np.array(fpga_energy)

        fig, ax1 = plt.subplots()

        index = np.arange(n_groups) 
        bar_width = 0.35
        opacity_front = 0.75
        opacity_back = 0.5

        rects.append(ax1.bar(index+bar_width, fpga_power_np, bar_width, alpha=opacity_front, color='b', label='FPGA Power', hatch = '/'))
        rects.append(ax1.bar(index+bar_width, total_power_np, bar_width, alpha=opacity_back, color='c', label='Total Power', hatch = '\\'))
        #rects.append(ax1.bar(index + bar_width, fpga_power, bar_width, alpha=opacity, color='c', label='FPGA Power'))
        


        ax1.set_xlabel('Configuration')
        ax1.set_ylabel('Power [W]')
        #ax1.set_title('')
        ax1.set_xticks(index + bar_width*3 / 2)
        
        config_labels = []
        for num, infile in enumerate(inputfile):
            config_labels.append(sched_type[num]+"+IOCTL(" + ioctl_flag[num] + "):"+str(cpu_cores[num])+"C+"+str(fpga_hpacc[num])+"HP+"+str(fpga_hpcacc[num])+"HPC")
        
        ax1.set_xticklabels(config_labels,fontsize='small',rotation='90')
        
        ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
        rects.append(ax2.bar(index + bar_width*2, total_energy_np, bar_width, alpha=opacity_front, color='r',label='Total Energy', hatch = '|'))
        rects.append(ax2.bar(index + bar_width*2, fpga_energy_np, bar_width, alpha=opacity_back, color='m', label='FPGA Energy', hatch = '-'))
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



