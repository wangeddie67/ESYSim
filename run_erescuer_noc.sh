rm paracfg.txt
offset=0
for bench in Uniform Transpose1 Transpose2 Bitreversal Butterfly Shuffle; do
	for intcycle in 20000 40000 60000 80000 100000 200000 500000 1000000; do
		echo "../../esynet/esynet -router_num 64 -array_size 8 8 -phy_number 7 -vc_number 1 -in_buffer_size 12 -out_buffer_size 12 -data_path_width 32 -link_length 1000 -routing_alg ERescuer -random_seed 1 -sim_length 100000 -simulation_period 1 -traffic_rule $bench -traffic_pir 0.05 -packet_size 5 -bist_enable -bist_type_code TARRA -bist_interval $intcycle -bist_timing 3000 -bist_offset $offset" >> paracfg.txt
	done
done
../../qtparallel/qtparallel -pc_num_avail 4 -arg_file_path paracfg.txt -out_file_path ../RESULT/result_erescuer_noc_${offset}.xls
