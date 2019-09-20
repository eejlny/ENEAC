<<<<<<< HEAD
# Author: Kris Nikov - kris.nikov@bris.ac.uk
# Date: 11 Jun 2019
# Description: Tcl commands to connect the 4 interrupt generators to the 4 HP accelerator design in vivado
=======
>>>>>>> pr/3
connect_bd_net [get_bd_pins sds_irq_const/dout] [get_bd_pins xlconcat_2/In4]
connect_bd_net [get_bd_pins sds_irq_const/dout] [get_bd_pins xlconcat_2/In5]
connect_bd_net [get_bd_pins sds_irq_const/dout] [get_bd_pins xlconcat_2/In6]
connect_bd_net [get_bd_pins sds_irq_const/dout] [get_bd_pins xlconcat_2/In7]
delete_bd_objs [get_bd_nets xlconstant_0_dout]
delete_bd_objs [get_bd_cells xlconstant_0]
connect_bd_net [get_bd_pins spmm_fast_hp01/ap_done] [get_bd_pins spmm_fast_hp01_if/ap_done]
connect_bd_net [get_bd_pins spmm_fast_hp01/ap_done] [get_bd_pins interrupt_generator_0/core_done]
connect_bd_net [get_bd_pins spmm_fast_hp01_if/ap_start] [get_bd_pins spmm_fast_hp01/ap_start]
connect_bd_net [get_bd_pins spmm_fast_hp01_if/ap_idle] [get_bd_pins spmm_fast_hp01/ap_idle]
connect_bd_net [get_bd_pins spmm_fast_hp01_if/ap_ready] [get_bd_pins spmm_fast_hp01/ap_ready]
connect_bd_net [get_bd_pins spmm_fast_hp12/ap_done] [get_bd_pins spmm_fast_hp12_if/ap_done]
connect_bd_net [get_bd_pins spmm_fast_hp12/ap_done] [get_bd_pins interrupt_generator_1/core_done]
connect_bd_net [get_bd_pins spmm_fast_hp12_if/ap_start] [get_bd_pins spmm_fast_hp12/ap_start]
connect_bd_net [get_bd_pins spmm_fast_hp12_if/ap_idle] [get_bd_pins spmm_fast_hp12/ap_idle]
connect_bd_net [get_bd_pins spmm_fast_hp12_if/ap_ready] [get_bd_pins spmm_fast_hp12/ap_ready]
connect_bd_net [get_bd_pins spmm_fast_hp23/ap_done] [get_bd_pins spmm_fast_hp23_if/ap_done]
connect_bd_net [get_bd_pins spmm_fast_hp23/ap_done] [get_bd_pins interrupt_generator_2/core_done]
connect_bd_net [get_bd_pins spmm_fast_hp23_if/ap_start] [get_bd_pins spmm_fast_hp23/ap_start]
connect_bd_net [get_bd_pins spmm_fast_hp23_if/ap_idle] [get_bd_pins spmm_fast_hp23/ap_idle]
connect_bd_net [get_bd_pins spmm_fast_hp23_if/ap_ready] [get_bd_pins spmm_fast_hp23/ap_ready]
connect_bd_net [get_bd_pins spmm_fast_hp34/ap_done] [get_bd_pins spmm_fast_hp34_if/ap_done]
connect_bd_net [get_bd_pins spmm_fast_hp34/ap_done] [get_bd_pins interrupt_generator_3/core_done]
connect_bd_net [get_bd_pins spmm_fast_hp34_if/ap_start] [get_bd_pins spmm_fast_hp34/ap_start]
connect_bd_net [get_bd_pins spmm_fast_hp34_if/ap_idle] [get_bd_pins spmm_fast_hp34/ap_idle]
connect_bd_net [get_bd_pins spmm_fast_hp34_if/ap_ready] [get_bd_pins spmm_fast_hp34/ap_ready]
save_bd_design