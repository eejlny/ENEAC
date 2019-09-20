#!/bin/bash
#SPMM wrap script for multipe data collection
#KrNikov 2019

dmesg -c > /dev/null

# #######
# #IOCTL#
# #######

# #CPUHPC
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 2 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_2chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 4 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_4chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 8 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_8chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 16 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_16chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 32 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_32chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 64 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_64chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 128 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_128chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 256 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_256chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 512 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_512chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 1024 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_1024chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 2048 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_2048chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 4096 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_4096chunk_ioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 8192 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_8192chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 16384 -d 1 -i -s results/89_fix_chunks/CPUHPC/IOCTL/4c_0hp_4hpc_16384chunk_ioctl.data

# #########
# #NOIOCTL#
# #########

# #CPUHPC
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 2 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_2chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 4 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_4chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 8 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_8chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 16 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_16chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 32 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_32chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 64 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_64chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 128 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_128chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 256 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_256chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 512 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_512chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 1024 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_1024chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 2048 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_2048chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 4096 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_4096chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 8192 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_8192chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 4 -k 16384 -d 1 -s results/89_fix_chunks/CPUHPC/NOIOCTL/4c_0hp_4hpc_16384chunk_noioctl.data

# #CPU
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 2 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_2chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 4 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_4chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 8 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_8chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 16 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_16chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 32 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_32chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 64 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_64chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 128 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_128chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 256 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_256chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 512 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_512chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 1024 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_1024chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 2048 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_2048chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 4096 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_4096chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 8192 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_8192chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -l 0 -k 16384 -d 1 -s results/89_fix_chunks/CPU/4c_0hp_0hpc_16384chunk_noioctl.data

# #HPC
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 2 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_2chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 4 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_4chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 8 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_8chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 16 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_16chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 32 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_32chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 64 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_64chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 128 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_128chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 256 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_256chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 512 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_512chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 1024 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_1024chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 2048 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_2048chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 4096 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_4096chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 8192 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_8192chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 4 -k 16384 -d 1 -s results/89_fix_chunks/HPC/0c_0hp_4hpc_16384chunk_noioctl.data

# #######
# #IDEAL#
# #######

# #CPU
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 2 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_2chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 4 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_4chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 8 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_8chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 16 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_16chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 32 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_32chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 64 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_64chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 128 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_128chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 256 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_256chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 512 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_512chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 1024 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_1024chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 2048 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_2048chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 4096 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_4096chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 8192 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_8192chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 1 -l 0 -k 16384 -d 1 -s results/89_fix_chunks/IDEAL/CPU/1c_0hp_0hpc_16384chunk_noioctl.data

# #HPC
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 2 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_2chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 4 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_4chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 8 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_8chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 16 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_16chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 32 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_32chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 64 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_64chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 128 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_128chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 256 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_256chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 512 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_512chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 1024 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_1024chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 2048 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_2048chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 4096 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_4096chunk_noioctl.data
# # ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 8192 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_8192chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -l 1 -k 16384 -d 1 -s results/89_fix_chunks/IDEAL/HPC/0c_0hp_1hpc_16384chunk_noioctl.data

#######
#IOCTL#
#######

# #CPUHP
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 2 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_2chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 4 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_4chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 8 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_8chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 16 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_16chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 32 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_32chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 64 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_64chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 128 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_128chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 256 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_256chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 512 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_512chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 1024 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_1024chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 2048 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_2048chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 4096 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_4096chunk_ioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 8192 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_8192chunk_ioctl.data
./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 16384 -d 1 -i -s results/89_fix_chunks/CPUHP/IOCTL/4c_4hp_0hpc_16384chunk_ioctl.data

#########
#NOIOCTL#
#########

# #CPUHP
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 2 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_2chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 4 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_4chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 8 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_8chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 16 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_16chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 32 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_32chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 64 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_64chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 128 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_128chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 256 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_256chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 512 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_512chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 1024 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_1024chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 2048 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_2048chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 4096 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_4096chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 8192 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_8192chunk_noioctl.data
./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 4 -p 4 -k 16384 -d 1 -s results/89_fix_chunks/CPUHP/NOIOCTL/4c_4hp_0hpc_16384chunk_noioctl.data

# #HP
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 2 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_2chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 4 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_4chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 8 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_8chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 16 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_16chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 32 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_32chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 64 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_64chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 128 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_128chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 256 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_256chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 512 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_512chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 1024 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_1024chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 2048 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_2048chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 4096 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_4096chunk_noioctl.data
# ./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 8192 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_8192chunk_noioctl.data
./spmm_fast.sh -n 7 -m matrices/mixtank_new.mtx -c 0 -p 4 -k 16384 -d 1 -s results/89_fix_chunks/HP/0c_4hp_0hpc_16384chunk_noioctl.data