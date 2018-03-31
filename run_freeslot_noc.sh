rm paracfg.txt
for bench in Uniform Transpose1 Transpose2 Bitreversal Butterfly Shuffle; do
	for intcycle in 20000 40000 60000 80000 100000 200000 500000 1000000; do
		echo "../../esynet/esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 2 -in_buffer_size 12 -out_buffer_size 12 -data_path_width 32 -link_length 1000 -routing_alg DyXY -random_seed 1 -sim_length 100000 -simulation_period 1 -bist_enable -bist_type_code Blocking -bist_interval $intcycle -bist_timing 2000 -traffic_rule $bench -traffic_pir 0.05" >> paracfg.txt
	done
#	for intcycle in       20000 40000 60000 80000 100000 200000 500000 1000000; do
#		echo "sh ${bench}.sh \"./esynet -router_num 80 -array_size 10 8 -phy_number 5 -vc_number 2 -in_buffer_size 12 -out_buffer_size 12 -data_path_width 32 -link_length 1000 -routing_alg DyXY -random_seed 1 -sim_length 10000 -simulation_period 1 -bist_enable -bist_type_code Blocking -bist_interval $intcycle -bist_timing 1000\" " >> paracfg.txt
#	done
done
../../qtparallel/qtparallel -pc_num_avail 4 -arg_file_path paracfg.txt -out_file_path ../RESULT/result_freeslot.xls


