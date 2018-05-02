#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Created on Sun Jun 9 2016

@author: asenjo
"""
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import sys


keys = set(['Step:','chunkCPU:'])

step=[]
chunkCPU = []
cpuTh = []
gpuTh = []
fG = []


with open('Dyn_cpuchunk2-1.txt') as f:
    for line in f:
        tokens = line.split()
        if len(tokens) and tokens[0] in keys:
            step.append(tokens[1])
            chunkCPU.append(tokens[3])
            cpuTh.append(tokens[5])
            gpuTh.append(tokens[7])
            fG.append(tokens[9])


serie1='chunkCPU'
serie2='cpuTh'
serie3='fpgaTh'
serie4='fG (ratio GPUth/CPUth)'
          
#Configuration variables
titlefs = 20
ylabelfs = 18
xlabelfs = 18
xticksfs = 16
yticksfs = 16
legendfs = 14
linew = 2
markers = 8
marks=['o-','x-','+-','v-','s-']


fig = plt.figure()
plt.plot(np.arange(len(step)), np.array(chunkCPU), marks[0], linewidth=linew, markersize=markers)

plt.title('Dynamic Study',  fontweight='bold', fontsize=titlefs)
plt.legend([serie1],loc='best', fontsize= legendfs)
plt.ylabel('Size (num bodies)', fontsize=ylabelfs)
plt.xlabel('samples', fontsize=xlabelfs)
plt.xticks(fontsize=xticksfs)
plt.yticks(fontsize=yticksfs, )
plt.grid()

plt.show()

pp = PdfPages('Dyn_chunkCPU.pdf')
pp.savefig(fig)
pp.close()

fig = plt.figure()
plt.plot(np.arange(len(step)), np.array(cpuTh), marks[0], linewidth=linew, markersize=markers)

plt.title('Dynamic Study',  fontweight='bold', fontsize=titlefs)
plt.legend([serie2],loc='best', fontsize= legendfs)
plt.ylabel('CPU Throughput (bodies/msec.)', fontsize=ylabelfs)
plt.xlabel('samples', fontsize=xlabelfs)
plt.xticks(fontsize=xticksfs)
plt.yticks(fontsize=yticksfs, )
plt.grid()

plt.show()

pp = PdfPages('Dyn_cpuTh.pdf')
pp.savefig(fig)
pp.close()

fig = plt.figure()
plt.plot(np.arange(len(step)), np.array(gpuTh), marks[0], linewidth=linew, markersize=markers)

plt.title('Dynamic Study',  fontweight='bold', fontsize=titlefs)
plt.legend([serie3],loc='best', fontsize= legendfs)
plt.ylabel('FPGA Throughput (bodies/msec.)', fontsize=ylabelfs)
plt.xlabel('samples', fontsize=xlabelfs)
plt.xticks(fontsize=xticksfs)
plt.yticks(fontsize=yticksfs, )
plt.grid()

plt.show()

pp = PdfPages('Dyn_fpgaTh.pdf')
pp.savefig(fig)
pp.close()

fig = plt.figure()
plt.plot(np.arange(len(step)), np.array(fG), marks[0], linewidth=linew, markersize=markers)

plt.title('Dynamic Study',  fontweight='bold', fontsize=titlefs)
plt.legend([serie4],loc='best', fontsize= legendfs)
plt.ylabel('relative speed FPGA/CPU', fontsize=ylabelfs)
plt.xlabel('samples', fontsize=xlabelfs)
plt.xticks(fontsize=xticksfs)
plt.yticks(fontsize=yticksfs, )
plt.grid()

plt.show()

pp = PdfPages('Dyn_fg.pdf')
pp.savefig(fig)
pp.close()


