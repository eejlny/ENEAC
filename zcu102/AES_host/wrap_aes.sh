#!/bin/bash
#AES wrap script for multipe data collection
#KrNikov 2019

dmesg -c > /dev/null

###############
###############
###############
###HPC PORTS###
###############
###############
###############

#######
#IOCTL#
#######

# #CPUHPC
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 2 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_2chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 4 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_4chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 8 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_8chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 16 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_16chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 32 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_32chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 64 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_64chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 128 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_128chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 256 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_256chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 512 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_512chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 1024 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_1024chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 2048 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_2048chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 4096 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_4096chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 8192 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_8192chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 16384 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_16384chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 32768 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_32768chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 65536 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_65536chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 131072 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_131072chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 262144 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_262144chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 524288 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_524288chunk_ioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 1048576 -d 1 -i -s results/9_recollect_hpc/CPUHPC/IOCTL/4c_0hp_4hpc_1048576chunk_ioctl.data

#########
#NOIOCTL#
#########

#CPUHPC
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 2 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_2chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 4 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_4chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 8 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_8chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 16 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_16chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 32 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_32chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 64 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_64chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 128 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_128chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 256 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_256chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 512 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_512chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 1024 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_1024chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 2048 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_2048chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 4096 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_4096chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 8192 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_8192chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 16384 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_16384chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 32768 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_32768chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 65536 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_65536chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 131072 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_131072chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 262144 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_262144chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 524288 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_524288chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -l 4 -k 1048576 -d 1 -s results/9_recollect_hpc/CPUHPC/NOIOCTL/4c_0hp_4hpc_1048576chunk_noioctl.data

#HPC
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 2 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_2chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 4 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_4chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 8 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_8chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 16 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_16chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 32 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_32chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 64 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_64chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 128 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_128chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 256 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_256chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 512 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_512chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 1024 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_1024chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 2048 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_2048chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 4096 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_4096chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 8192 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_8192chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 16384 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_16384chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 32768 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_32768chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 65536 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_65536chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 131072 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_131072chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 262144 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_262144chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 524288 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_524288chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 4 -k 1048576 -d 1 -s results/9_recollect_hpc/HPC/0c_0hp_4hpc_1048576chunk_noioctl.data

#######
#IDEAL#
#######

#HPC
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 2 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_2chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 4 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_4chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 8 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_8chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 16 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_16chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 32 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_32chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 64 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_64chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 128 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_128chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 256 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_256chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 512 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_512chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 1024 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_1024chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 2048 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_2048chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 4096 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_4096chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 8192 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_8192chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 16384 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_16384chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 32768 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_32768chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 65536 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_65536chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 131072 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_131072chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 262144 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_262144chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 524288 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_524288chunk_noioctl.data
./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -l 1 -k 1048576 -d 1 -s results/9_recollect_hpc/IDEAL/HPC/0c_0hp_1hpc_1048576chunk_noioctl.data

# ##############
# ##############
# ##############
# ###HP PORTS###
# ##############
# ##############
# ##############

# #######
# #IOCTL#
# #######

# #CPUHP
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 2 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_2chunk_ioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 4 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_4chunk_ioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 8 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_8chunk_ioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 16 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_16chunk_ioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 32 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_32chunk_ioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 64 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_64chunk_ioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 128 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_128chunk_ioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 256 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_256chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 512 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_512chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 1024 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_1024chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 2048 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_2048chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 4096 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_4096chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 8192 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_8192chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 16384 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_16384chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 32768 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_32768chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 65536 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_65536chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 131072 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_131072chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 262144 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_262144chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 524288 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_524288chunk_ioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 1048576 -d 1 -i -s results/9_recollect_hpc/CPUHP/IOCTL/4c_4hp_0hpc_1048576chunk_ioctl.data

# #########
# #NOIOCTL#
# #########

# #CPU
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 2 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_2chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 4 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_4chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 8 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_8chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 16 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_16chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 32 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_32chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 64 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_64chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 128 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_128chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 256 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_256chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 512 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_512chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 1024 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_1024chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 2048 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_2048chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 4096 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_4096chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 8192 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_8192chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 16384 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_16384chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 32768 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_32768chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 65536 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_65536chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 131072 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_131072chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 262144 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_262144chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 524288 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_524288chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 0 -k 1048576 -d 1 -s results/9_recollect_hpc/CPU/4c_0hp_0hpc_1048576chunk_noioctl.data

# #HP
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 2 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_2chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 4 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_4chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 8 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_8chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 16 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_16chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 32 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_32chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 64 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_64chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 128 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_128chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 256 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_256chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 512 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_512chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 1024 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_1024chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 2048 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_2048chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 4096 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_4096chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 8192 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_8192chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 16384 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_16384chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 32768 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_32768chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 65536 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_65536chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 131072 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_131072chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 262144 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_262144chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 524288 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_524288chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 0 -p 4 -k 1048576 -d 1 -s results/9_recollect_hpc/HP/0c_4hp_0hpc_1048576chunk_noioctl.data

# #CPUHP
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 2 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_2chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 4 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_4chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 8 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_8chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 16 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_16chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 32 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_32chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 64 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_64chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 128 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_128chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 256 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_256chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 512 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_512chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 1024 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_1024chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 2048 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_2048chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 4096 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_4096chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 8192 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_8192chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 16384 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_16384chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 32768 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_32768chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 65536 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_65536chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 131072 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_131072chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 262144 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_262144chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 524288 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_524288chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 4 -p 4 -k 1048576 -d 1 -s results/9_recollect_hpc/CPUHP/NOIOCTL/4c_4hp_0hpc_1048576chunk_noioctl.data

# #######
# #IDEAL#
# #######

# #CPU
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 2 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_2chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 4 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_4chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 8 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_8chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 16 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_16chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 32 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_32chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 64 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_64chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 128 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_128chunk_noioctl.data
# # ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 256 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_256chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 512 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_512chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 1024 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_1024chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 2048 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_2048chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 4096 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_4096chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 8192 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_8192chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 16384 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_16384chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 32768 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_32768chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 65536 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_65536chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 131072 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_131072chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 262144 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_262144chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 524288 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_524288chunk_noioctl.data
# ./aes.sh -n 7 -f sydney_fpga.pdf -c 1 -p 0 -k 1048576 -d 1 -s results/9_recollect_hpc/IDEAL/CPU/1c_0hp_0hpc_1048576chunk_noioctl.data