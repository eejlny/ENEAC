# Author: Kris Nikov - kris.nikov@bris.ac.uk
# Date: 11 Jun 2019
# Description: Tcl commands to connect 200MHz clkwiz to the 4 HPC accelerator design in vivado
delete_bd_objs [get_bd_nets ps_e_pl_clk0]
create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wiz_0
set_property -dict [list CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {200.000} CONFIG.USE_LOCKED {false} CONFIG.USE_RESET {false} CONFIG.MMCM_DIVCLK_DIVIDE {1} CONFIG.MMCM_CLKOUT0_DIVIDE_F {6.000} CONFIG.CLKOUT1_JITTER {102.086}] [get_bd_cells clk_wiz_0]
set_property location {3 1226 482} [get_bd_cells clk_wiz_0]
connect_bd_net [get_bd_pins ps_e/pl_clk0] [get_bd_pins clk_wiz_0/clk_in1]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins rst_ps_e_99M/slowest_sync_clk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins interrupt_generator_0/ip_clk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins interrupt_generator_0/s00_axi_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins interrupt_generator_1/ip_clk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins interrupt_generator_1/s00_axi_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins interrupt_generator_2/ip_clk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins interrupt_generator_2/s00_axi_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins interrupt_generator_3/ip_clk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins interrupt_generator_3/s00_axi_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps_e/maxihpm0_fpd_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps_e/maxihpm0_lpd_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps_e/saxihpc0_fpd_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps_e/saxihpc1_fpd_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps_e_axi_periph/ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps_e_axi_periph/M00_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps_e_axi_periph/M01_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps_e_axi_periph/M02_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps_e_axi_periph/M03_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins spmm_fast_hpc01_if/s_axi_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins spmm_fast_hpc01_if/acc_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins spmm_fast_hpc12_if/s_axi_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins spmm_fast_hpc12_if/acc_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins spmm_fast_hpc23_if/s_axi_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins spmm_fast_hpc23_if/acc_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins spmm_fast_hpc34_if/s_axi_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins spmm_fast_hpc34_if/acc_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_M_AXI_HPM0_FPD/ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps_e_axi_periph/S00_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_M_AXI_HPM0_FPD/S00_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_M_AXI_HPM0_FPD/M00_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_M_AXI_HPM0_FPD/M01_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_M_AXI_HPM0_FPD/M02_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_M_AXI_HPM0_FPD/M03_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/S00_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/M00_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/S01_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/S02_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/S03_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/S04_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/S05_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/S06_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/S07_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/S08_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC0_FPD/S09_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/S00_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/M00_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/S01_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/S02_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/S03_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/S04_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/S05_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/S06_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/S07_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/S08_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins axi_ic_ps_e_S_AXI_HPC1_FPD/S09_ACLK]
save_bd_design
validate_bd_design