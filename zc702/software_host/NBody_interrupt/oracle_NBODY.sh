#!/bin/bash
etapa=NADA

function pausa {
	
	echo "Buscamos /home/odroid/lock ..."
	date
	sleep 5
	while [ -f /home/odroid/lock ]; do 
	echo "Fichero encontrado: pausa de 5 minutos"
	date
	sleep 300 
	done
	echo "continuamos..."
             
           }
           
function aviso {
	echo "Estamos en $0: $etapa" | mail -s "Aviso desde la Odroid: $0 - $etapa" andres.rdgz@gmail.com
}

function fin {
	etapa=FIN
	aviso
	exit
}

#---------------------------

           
bench=NBODY
cd /home/odroid/parallel_for_linux/${bench}  




etapa=oracle
aviso 

echo "start ${etapa} ; -----------------------------------------" >> ${bench}.csv
echo `date` >> ${bench}.csv
echo "start ${etapa};"
echo `date`

prog=./${bench}_SS


for cpus in 3 4; do
for x in "0" "0.1" "0.2" "0.3" "0.4" "0.5" "0.6" "0.7" "0.8" "0.9" "1.0"; do
for i in 1 2 3 4 5; do
echo ${prog} ${cpus} ${x}
   taskset -c 4-7 ${prog} Data/runB.in ${cpus} 1 ${x}
    pausa
done
done
done



etapa=TODOS
aviso 

for prog in ./${bench} ./${bench}_A7 ./${bench}_PRIO ./${bench}_A7_PRIO  ; do
for cpus in 3 4 7 8; do
for i in 1 2 3 4 5; do
echo ${prog} ${cpus}
 ${prog} Data/runB.in ${cpus} 1  1024 2> /dev/null
   pausa
done
done
done

fin


etapa=blocking
aviso 

echo "start ${etapa} ; -----------------------------------------" >> ${bench}.csv
echo `date` >> ${bench}.csv
echo "start ${etapa};"
echo `date`

prog=./${bench}_b


for block in 128 512 1024 2048 4096 8192 16384 32768 65536 100000 ; do
for i in 1 2 3 4 5; do
echo ${prog} ${cpus}
  taskset -c 4-7 ${prog} Data/runB.in 0 1 ${block} 
   pausa

done
done

