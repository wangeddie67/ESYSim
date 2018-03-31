rm paracfg.txt
for bench in Uniform Transpose1 Transpose2 Bitreversal Butterfly Shuffle; do
	for intcycle in 20000 40000 60000 80000 100000 200000 500000 1000000; do
		echo "../../esynet/esynet -network_cfg_file_enable -router_cfg_file_name /home/wjsh/esysim-opensource/example/mesh_nonblocking_parsec -data_path_width 32 -link_length 1000 -routing_alg NonBlock -random_seed 1 -sim_length 100000 -simulation_period 1 -traffic_rule $bench -traffic_pir 0.05 -packet_size 5 -bist_enable -bist_type_code NonBlock -bist_interval $intcycle -bist_timing 2000 1000 1000 -bist_flit 1 40" >> paracfg.txt
	done
	for intcycle in  20000 40000 60000 80000 100000 200000 500000 1000000; do
#	for intcycle in  20000; do
		echo "../../esynet/esynet -network_cfg_file_enable -router_cfg_file_name /home/wjsh/esysim-opensource/example/mesh_nonblocking_parsec -data_path_width 32 -link_length 1000 -routing_alg NonBlock -random_seed 1 -sim_length 100000 -simulation_period 1 -traffic_rule $bench -traffic_pir 0.05 -packet_size 5 -bist_enable -bist_type_code NonBlock -bist_interval $intcycle -bist_timing 2000 1000 1000 -bist_flit 40 1" >> paracfg.txt
	done
done
../../qtparallel/qtparallel -pc_num_avail 4 -arg_file_path paracfg.txt -out_file_path ../RESULT/result_noblocking_trace.xls

