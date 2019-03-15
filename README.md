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
