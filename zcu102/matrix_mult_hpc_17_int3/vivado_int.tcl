open_project /home/eejlny/projects/sdsoc/matrix_mult_hpc_17_int3/_sds/p0/ipi/zcu102_es2.xpr
open_bd_design {/home/eejlny/projects/sdsoc/matrix_mult_hpc_17_int3/_sds/p0/ipi/zcu102_es2.srcs/sources_1/bd/zcu102_es2/zcu102_es2.bd}
disconnect_bd_net /xlconstant_0_dout [get_bd_pins xlconcat_1/In0]
connect_bd_net [get_bd_pins mmult_top_1/ap_done] [get_bd_pins xlconcat_1/In0]
connect_bd_net [get_bd_pins mmult_top_1_if/ap_done] [get_bd_pins mmult_top_1/ap_done]
disconnect_bd_net /ps_e_pl_clk0 [get_bd_pins interrupt_generator_0/ip_clk]
connect_bd_net [get_bd_pins mmult_top_1_if/ap_clk] [get_bd_pins interrupt_generator_0/ip_clk]
save_bd_design
reset_run synth_1
launch_runs impl_1 -to_step write_bitstream -jobs 4
wait_on_run impl_1 -quiet