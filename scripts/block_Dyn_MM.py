
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
    return (dataCores)
    #return (extrae_medianas(indice,columna,dataCores))
    

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
fig_width = 14
fig_height = 6

colorcycle = ['#a1dab4', '#41b6c4', '#2c7fb8', '#253494',]
          

############################
# Carga fichero
############################


file0='_Dyn.C0.G1.txt'
file1='_Dyn.C1.G1.txt'
file2='_Dyn.C2.G1.txt'
file3='_Dyn.C3.G1.txt'
file4='_Dyn.C4.G1.txt'
file5='_Dyn.C1.G0.txt'
file6='_Dyn.C2.G0.txt'
file7='_Dyn.C3.G0.txt'
file8='_Dyn.C4.G0.txt'


#blocks= carga_fichero(file1,'\t',1,4)

# indices empiezan en cero
# parametros:  fichero, delimitador, header?, indice unificador, valor
time_b0=carga_fichero(file0,'\t',1,2,3)  
time_b1=carga_fichero(file1,'\t',1,2,3)  
time_b2=carga_fichero(file2,'\t',1,2,3)  
time_b3=carga_fichero(file3,'\t',1,2,3)
time_b4=carga_fichero(file4,'\t',1,2,3)   
time_b5=carga_fichero(file5,'\t',1,2,3)
time_b6=carga_fichero(file6,'\t',1,2,3)
time_b7=carga_fichero(file7,'\t',1,2,3)
time_b8=carga_fichero(file8,'\t',1,3,4)
  
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



#theoretical max troghput at 100 Mhz in FPGA is  10e7 (100 million per second)

#xx = time_b0[0:5,2]
#xx = np.array([1,2,3,4,5,6])
#xx = np.array([1,2,3,4,5])
#my_xticks = ['32','64','128','256', '512']
xx = np.array([1,2,3,4])
my_xticks = ['32','64','128','256']
yy0 = time_b0[0:4,3]
yy1 = time_b1[0:4,3]
yy2 = time_b2[0:4,3]
yy3 = time_b3[0:4,3]
yy4 = time_b4[0:4,3]
yye0 = time_b0[0:4,8]
yye1 = time_b1[0:4,8]
yye2 = time_b2[0:4,8]
yye3 = time_b3[0:4,8]
yye4 = time_b4[0:4,8]
yy5 = time_b5[0:4,3]
yy6 = time_b6[0:4,3]
yy7 = time_b7[0:4,3]
yy8 = time_b8[0:4,3]


#yy12 = time_b12[:,4]
#yy13 = time_b13[:,4]
#yy14 = time_b14[:,4]



figure1='T_MM_dynamic_1024.pdf'
title1='T_MM_dynamic_SDSOC_1024'

figure2='E_MM_dynamic_1024.pdf'
title2='E_MM_dynamic_SDSOC_1024'

serie1='4 FC'
serie2='4 FC + 1 CC'
serie3='4 FC + 2 CC'
serie4='4 FC + 3 CC'
serie5='4 FC + 4 CC'
serie6='1 CC'
serie7='2 CC'
serie8='3 CC'
serie9='4 CC'

#plt.plot(xx, (m000[0,10]/m000[:,10]), 'o-', linewidth=linew, markersize=markers)
#plt.plot(xx, (m000[0,10]/m101[:,10]), 'v-', linewidth=linew, markersize=markers)
#plt.plot(xx, (m000[0,10]/m111[:,10]), 's-', linewidth=linew, markersize=markers)
#plt.plot(xx, (m000[0,10]/m0111[:,10]), '1-', linewidth=linew, markersize=markers)

#plt.ticklabel_format(style='sci', axis='y', scilimits=(0,0))

matrix_size = 1024
plt.plot(xx, 1000*(matrix_size*matrix_size)/(yy0), 'o-', linewidth=linew, markersize=markers)
plt.plot(xx, 1000*(matrix_size*matrix_size)/(yy1), 'v-', linewidth=linew, markersize=markers)
plt.plot(xx, 1000*(matrix_size*matrix_size)/(yy2), 's-', linewidth=linew, markersize=markers)
plt.plot(xx, 1000*(matrix_size*matrix_size)/(yy3), '1-', linewidth=linew, markersize=markers)
plt.plot(xx, 1000*(matrix_size*matrix_size)/(yy4), '2-', linewidth=linew, markersize=markers)
plt.plot(xx, 1000*(matrix_size*matrix_size)/(yy5), 's-', linewidth=linew, markersize=markers)
plt.plot(xx, 1000*(matrix_size*matrix_size)/(yy6), 'v-', linewidth=linew, markersize=markers)
plt.plot(xx, 1000*(matrix_size*matrix_size)/(yy7), '1-', linewidth=linew, markersize=markers)
plt.plot(xx, 1000*(matrix_size*matrix_size)/(yy8), '1-', linewidth=linew, markersize=markers)



#logfit=[100000.0*15.0/(10382.0/1000),702.77, 100000.0*15.0/(15135.5/1000)]
#plt.plot([8200], logfit[0], 'o-', linewidth=linew, markersize=markers)
#plt.plot([8200], logfit[2], 'o-', linewidth=linew, markersize=markers)



#plt.ylim((0,10000))
plt.grid()
#plt.title(title1,  fontweight='bold', fontsize=titlefs)
#serie1, serie2, serie3,serie4,serie5,
#my_xticks = ['128','256','512','1024','2048']
#x = np.array([8,16,32,64,128,256,512])
plt.legend([serie1, serie2, serie3,serie4,serie5,serie6,serie7,serie8,serie9],loc='best', fontsize= legendfs)
plt.ylabel('Throughput (matrix elements/s)', fontsize=ylabelfs)
plt.xlabel('block size', fontsize=xlabelfs)
plt.xticks(xx, my_xticks, fontsize=xticksfs)
#plt.xticks(fontsize=xticksfs)
plt.yticks(fontsize=yticksfs)

#plt.xscale('log')

plt.show()

pp = PdfPages(figure1)
pp.savefig(fig)
pp.close()




