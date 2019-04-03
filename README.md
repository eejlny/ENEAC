# gphcu
General Purpose Heterogenous Computing Units

Heterogeneous chips that combine CPUs and FPGAs can distribute processing so that the algorithm tasks are mapped onto the most suitable processing
element. New software-defined high-level design environments for these chips use general purpose languages such as C++ and OpenCL for hardware and interface generation without the need for register transfer language expertise. These advances in
hardware compilers have resulted in significant increases in FPGA design productivity.
In this research, we investigate how to enhance an existing software-defined framework to reduce overheads and enable the utilization of all the available CPU cores in parallel
with the FPGA hardware accelerators. Instead of selecting the best processing element for a task and simply offloading onto it, we introduce two schedulers, Dynamic and
LogFit, which distribute the tasks among all the resources in an optimal manner. A new platform is created based on interrupts that removes spin-locks and allows the processing cores to sleep when not performing useful work. For a compute-intensive
application, we obtained up to 45.56% more throughput and 17.89% less energy consumption when all devices of a Zynq-7000 SoC collaborate in the computation
compared against FPGA-only execution.

HYPERTHREADING in ultrascale.
The ultrascale heterogeonous system uses a hyperthreading technique for the PL cores. While in previous work there is a single hardware thread that is controlled by the scheduler and then this activates up to 4 FPGA cores in parallel and waits for all of them to complete before continuing. All the hardware cores are activated in parallel at roughly the same time and they get the same proportion of the data space to compute. This means that if one of the cores computes faster because, for example, computation time depends on data contents and not just the amount of data then this core has to wait for the rest to finish. With hardware hyperthreading each core behaves as a virtual independedent device and the scheduler can assign work to this device indepedently of the state of computation in the other FGPA cores. For hardware hyperthreading each hardware instance is exposed to the scheduler through a different function prototype. Two variations are available using the resource pragmas or with multiple hardware functions.

Using resource. In this scenario there is only 1 hardware function and it is instantiated multiple times in the kernel function with the addition of a resource pragma so multiple hardware instances are created. The pragmas control the interface ports to be used and the hardware instances share these ports. Example :
//hardware instance approach used in GEMM
  #pragma SDS resource(1)
   mmult_top(array_temp_a, array_temp_b, array_temp_c, line_count);

Multiple fuctions. In this scenario each hardware instance have a different hardware function although they all performance the same functionality. This approach requires the work of repliacting the function code but it has the advantage that each instance can use different pragmas to control the interface types. So for example instance 1 can use only high performance port 1 etc.  In principle both solutions offer the same performance but it is easier to identify problems with ports since they are not shared. Example:
//hardware instance approach used in SPMM
   mmult_top1(array_temp_a, array_temp_b, array_temp_c, line_count);

NOTE: If you use HP ports to interface the hardware then execution with more than one FPGA core fails with no virtual memory mapping. This is not a problem with HPC. If you really want to use the HP ports then you need make independent SD_alloc memory allocations per core (not a single area)  and copy chuncks of data from malloc to sds_alloc on-demand. It seems that with HP ports the 4 hardware cores cannot access a single sds_alloc area. 

ENERGY METERING SUPPORT: to add energy/power metering support replace the scheduler files with the files located in the energy-support directory and also move the directory energy-meter to the same level as the benchmark_host directory. The scheduler.h should have #define ENERGY and the Makefile will compile and link with the energy-mter library.

SCRIPTS: the script folder contains an example of the data that should be extracted from the board and the processing of this data to generate results figures. Modify as needed.


















