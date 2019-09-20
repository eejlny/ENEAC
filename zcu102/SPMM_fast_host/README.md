# SPMV_SDSOC

Fast implementation of SPMV with SDSOC C++ targetting Zynq Ultrascale device. 

Some results:

      1 CPU (ms)   4 CPU (ms)  1 FPGA (ms) 4 FPGA (ms) NNZ                                     
ROTOR1 0.013         0.0077     0.007        0.008   708(100x100)                         
FPGA_DCOP_11 0.107    0.050     0.047        0.02 5897(1220x1220)                   
BEACXC      N/A        N/A       0.36       0.15  50409(497x506)                      
MIXTANK_NEW 36.28      13.8    14.6     4.6  1990919(29957x29957) 



CPU CLK freq 1200 MHz
FPGA CLK freq 200 MHz

 

Wait overheads Hypothesis: 
1.If you launch the kernels asynchronously at some point you need to use wait. You wait for 1 kernel then 2 kernel then 3 kernel and finally 4 kernels. After calling WAIT means that 1 CPU core is fully used waiting for the first kernel, then the same core waits for the second kernel (that naturally should finish close to the first kernel so waiting is small) and so on. The end result is that 1 core (25% of CPU is always used and this is the sdsoc overhead). The same happens if you launch the kernels one a time WITHOUT async launch. The CPU is fully used waiting for the first kernel to finish and then it moves to launch the second kernel. Of course, this is much slower from a computation time point of view that the async launch since only one kernel computes at a time but the overhead in the CPU is the same 1 core = 25%.  This has been shown in the experimental phase so far.
Basically in this case there is one WAIT active and consumes one CPU.
2.When the schedulers create independent hardware threads, each of these threads has access to an independent compute unit. The compute unit does not use async launch and the hardware thread of the scheduler waits for completion. Async launch is pointless in this configuration since the scheduler sees independent compute units (like independent GPUs) that it manages and waits for the execution to finish before assigning more work to each compute unit independently.
In this case 2 all the compute unit are working in parallel so it is a fast solution similar to 1 with ASYNC launch but the overheads of waiting for the compute units to finish are going to increase. The idea is that in case 1 only 1 WAIT is active but now all WAIT are active since all are waiting for completion in parallel. So the hypothesis is that we will see the %CPU used during this waiting increase and a interrupt solution will be even more important to avoid these SDSOC wait overheads.
             


                                 

