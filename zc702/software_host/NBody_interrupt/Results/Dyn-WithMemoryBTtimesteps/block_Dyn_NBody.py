
##############################################################################
# Adapted from: Antonio Vilches
# Revised by:   Andres Rodriguez for ViVid
# Version:      1.1
# Date:         7/32/2015
# Description:  Plotting script fro ViVid on Odroid
# Copyright:    Department Computer Architecture at University of Malaga
##############################################################################

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from matplotlib.ticker import ScalarFormatter
import matplotlib.patches as mpatches


def f(): raise Exception("Found exit()")
    
    
def extrae_medianas(indice,valor,matriz):
    inicio=0
    fin=0
    limite=matriz.shape[0]
    salida=[]
    while inicio<limite:
        indice_actual=matriz[inicio,indice]
        fin=inicio
        while fin<limite and matriz[fin,indice]==indice_actual:
            fin=fin+1
        mediana=np.median(matriz[inicio:fin,valor])
        good_pointer=inicio
        diferencia=100000
        for pointer in range(inicio,fin):
            if abs(matriz[pointer,valor]-mediana)<diferencia:
                diferencia=abs(matriz[pointer,valor]-mediana)
                good_pointer=pointer
        salida.append(matriz[good_pointer,:])
        inicio=fin+1
    salida=np.array(salida)
    return salida
    
def carga_fichero(file_name,delim,header, indice, columna):
    dataCores = []
    dataCores.append(np.genfromtxt(file_name, delimiter=delim, skip_header=header))
    dataCores = np.array(dataCores)[0,:,:]  # porque tiene 3 dimensiones???
    return (extrae_medianas(indice,columna,dataCores))
    

##############################################################################
# Main script
##############################################################################
################################################
#Configuration variables
################################################
titlefs = 20
ylabelfs = 20
xlabelfs = 20
xticksfs = 18
yticksfs = 18
legendfs = 12
linew = 3
markers = 12
fig_width = 8
fig_height = 6

colorcycle = ['#a1dab4', '#41b6c4', '#2c7fb8', '#253494']


############################
# Carga fichero
############################


file0='NBody_Dyn.C0.G1.txt'
file1='NBody_Dyn.C1.G1.txt'
file2='NBody_Dyn.C2.G1.txt'

#blocks= carga_fichero(file1,'\t',1,4)

# indices empiezan en cero
# parametros:  fichero, delimitador, header?, indice unificador, valor
time_b0=carga_fichero(file0,'\t',1,2,3)  
time_b1=carga_fichero(file1,'\t',1,2,3)  
time_b2=carga_fichero(file2,'\t',1,2,3)  
 

#time_b12=carga_fichero(file12,'\t',1,3,4)  
#time_b13=carga_fichero(file13,'\t',1,3,4)  
#time_b14=carga_fichero(file14,'\t',1,3,4)  

#file5='NBody_PRIO_LOGFIT_3_1.txt'
#file6='NBody_PRIO_LOGFIT_4_1.txt'

#timel3 = carga_fichero(file5,'\t',1,1,2)
#timel4 = carga_fichero(file5,'\t',1,1,2)

################################################
# Blocking GPU only
################################################

fig = plt.figure()
fig.set_size_inches(fig_width, fig_height)

xx = time_b0[0:9,2]
yy0 = time_b0[0:9,3]
yy1 = time_b1[0:9,3]
yy2 = time_b2[0:9,3]

#yy12 = time_b12[:,4]
#yy13 = time_b13[:,4]
#yy14 = time_b14[:,4]



figure1='NBody_dynamic.pdf'
title1='Nbody Dynamic sched'

serie1='FPGA ONLY'
serie2='FPGA + 1 CORE'
serie3='FPGA + 2 CORES'

#plt.plot(xx, (m000[0,10]/m000[:,10]), 'o-', linewidth=linew, markersize=markers)
#plt.plot(xx, (m000[0,10]/m101[:,10]), 'v-', linewidth=linew, markersize=markers)
#plt.plot(xx, (m000[0,10]/m111[:,10]), 's-', linewidth=linew, markersize=markers)
#plt.plot(xx, (m000[0,10]/m0111[:,10]), '1-', linewidth=linew, markersize=markers)


plt.plot(xx, 10000*5/(yy0/1000), 'o-', linewidth=linew, markersize=markers)
plt.plot(xx,10000*5/( yy1/1000), 'v-', linewidth=linew, markersize=markers)
plt.plot(xx, 10000*5/(yy2/1000), 's-', linewidth=linew, markersize=markers)



#logfit=[100000.0*15.0/(10382.0/1000),702.77, 100000.0*15.0/(15135.5/1000)]
#plt.plot([8200], logfit[0], 'o-', linewidth=linew, markersize=markers)
#plt.plot([8200], logfit[2], 'o-', linewidth=linew, markersize=markers)



plt.grid()
plt.title(title1,  fontweight='bold', fontsize=titlefs)
#serie1, serie2, serie3,serie4,serie5,
plt.legend([serie1, serie2, serie3],loc='best', fontsize= legendfs)
plt.ylabel('Throughput (bodies/s)', fontsize=ylabelfs)
plt.xlabel('block size', fontsize=xlabelfs)
plt.xticks( fontsize=xticksfs)
plt.yticks(fontsize=yticksfs)


plt.xscale('log')

plt.show()

pp = PdfPages(figure1)
pp.savefig(fig)
pp.close()



################################################
# Blocking GPU only
################################################

fig = plt.figure()
fig.set_size_inches(fig_width, fig_height)


xx = time_b0[0:9,2]
yy0 = time_b0[0:9,7]
yy1 = time_b1[0:9,7]
yy2 = time_b2[0:9,7]



figure1='Energy NBody Dynamic.pdf'
title1='Energy NBody Dynamic sched'


serie1='FPGA ONLY'
serie2='FPGA + 1 CORE'
serie3='FPGA + 2 CORES'
serie4='FPGA + 3 CORES'
serie5='FPGA + 4 CORES'
serie6='LOGFIT 4+1'

#plt.plot(xx, (m000[0,10]/m000[:,10]), 'o-', linewidth=linew, markersize=markers)

#plt.plot(xx, (m000[0,10]/m101[:,10]), 'v-', linewidth=linew, markersize=markers)

#plt.plot(xx, (m000[0,10]/m111[:,10]), 's-', linewidth=linew, markersize=markers)
#plt.plot(xx, (m000[0,10]/m0111[:,10]), '1-', linewidth=linew, markersize=markers)

plt.plot(xx, yy0, 'o-', linewidth=linew, markersize=markers)
plt.plot(xx, yy1, 'v-', linewidth=linew, markersize=markers)
plt.plot(xx, yy2, 's-', linewidth=linew, markersize=markers)


#plt.plot(xx, zz, 'x-', linewidth=linew, markersize=markers)

plt.grid()
plt.title(title1,  fontweight='bold', fontsize=titlefs)
plt.legend([serie1, serie2,serie3],loc='best', fontsize= legendfs)
plt.ylabel('Energy (J)', fontsize=ylabelfs)
plt.xlabel('block size', fontsize=xlabelfs)
plt.xticks( fontsize=xticksfs)
plt.yticks(fontsize=yticksfs)


plt.xscale('log')

plt.show()

pp = PdfPages(figure1)
pp.savefig(fig)
pp.close()




################################################
#Plotting energy figure
################################################


figure1='Energy NBody Dynamic_bars.pdf'
title1='Energy NBody Dynamic sched'


width = 0.90/5
fig = plt.figure()
fig.set_size_inches(fig_width*1.5, fig_height)
x = np.arange(1, len(yy0)+1)

plt.bar(x, time_b0[0:9,4], width, color=colorcycle[1], hatch='..')
plt.bar(x, time_b0[0:9,5], width, bottom=time_b0[0:9,4], color=colorcycle[0], hatch='..')

plt.bar(x+1*width, time_b1[0:9,4], width, color=colorcycle[1], hatch='//')
plt.bar(x+1*width, time_b1[0:9,5], width, bottom=time_b1[0:9,4], color=colorcycle[0], hatch='//')
plt.bar(x+2*width, time_b2[0:9,4], width, color=colorcycle[1], hatch=' ')
plt.bar(x+2*width, time_b2[0:9,5], width, bottom=time_b2[0:9,4], color=colorcycle[0], hatch=' ')

plt.grid()

plt.title(title1,  fontweight='bold', fontsize=titlefs)

patch1 = mpatches.Patch( hatch='..', linestyle='solid',fill=False, label=serie1)
patch000 = mpatches.Patch( hatch='//', linestyle='solid',fill=False, label=serie2)
patch101 = mpatches.Patch( hatch=' ',fill=False, linestyle='solid',label=serie3)

first_legend=plt.legend(handles=[patch1,patch000,patch101],loc='upper right',fontsize= legendfs)
# Add the legend manually to the current Axes.
#ax = plt.gca().add_artist(handles=first_legend)


#plt.legend(['CPUs','FPGA'], loc='lower right', fontsize= legendfs)

#patch000 = mpatches.Patch(color=colorcycle[3], label='Uncore')
#patch101 = mpatches.Patch(color=colorcycle[2], label='GPU')
patch111 = mpatches.Patch(color=colorcycle[1], label='CPU')
patch1111 = mpatches.Patch(color=colorcycle[0], label='FPGA')

plt.legend(handles=[patch111,patch1111],loc='upper center',fontsize= legendfs)

plt.ylabel('Consumed Energy (J)', fontsize=ylabelfs)
plt.xlabel('blocksize', fontsize=xlabelfs)
plt.xticks(x+2*width, ['128', '256', '512', '1K', '2K', '4K','8K', '10k'], fontsize=xticksfs)
plt.yticks(fontsize=yticksfs)

plt.show()

pp = PdfPages(figure1)
pp.savefig(fig)
pp.close()



