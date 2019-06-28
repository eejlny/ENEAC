#all: meter libenergy_meter.so 
all: libenergy_meter_arm.a

#libenergy_meter.a: energy_meter.o
#	ar rcs ../lib/libenergy_meter.a energy_meter.o

libenergy_meter.so: energy_meter_s.o energy_meter_sn.o
	gcc -shared -o ../lib/libenergy_meter.so energy_meter_s.o
	gcc -shared  -mfloat-abi=softfp  -mfpu=neon -o ../lib/libenergy_meter_neon.so energy_meter_sn.o

meter: main_energy_meter.c energy_meter.o 
	gcc main_energy_meter.c energy_meter.o -pthread -o meter

energy_meter_s.o: energy_meter.c energy_meter.h
	gcc -c -fPIC energy_meter.c -o energy_meter_s.o
energy_meter_sn.o: energy_meter.c energy_meter.h
	gcc -c -fPIC -mfloat-abi=softfp  -mfpu=neon energy_meter.c -o energy_meter_sn.o

energy_meter.o: energy_meter.c energy_meter.h
	gcc -c energy_meter.c -o energy_meter.o

libenergy_meter_arm.a: energy_meter.c energy_meter.h
	gcc -O3 -c energy_meter.c -o energy_meter_arm.o
	ar rcs libenergy_meter_arm.a energy_meter_arm.o
	cp libenergy_meter_arm.a ../lib_energy

