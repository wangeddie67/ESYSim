rm paracfg.txt
for bench in Uniform Transpose1 Transpose2 Bitreversal Butterfly Shuffle; do
	echo "./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 2 -in_buffer_size 12 -out_buffer_size 12 -data_path_width 32 -link_length 1000 -traffic_rule ${bench} -traffic_pir 0.03 -routing_alg DyXY -random_seed 1 -sim_length 10000 -simulation_period 1 -event_trace_enable -event_trace_file_name ../example/event_${bench} -event_trace_file_text_enable" >> paracfg.txt
done
../qtparallel/qtparallel -pc_num_avail 3 -arg_file_path paracfg.txt -out_file_path ../example/result_notest.xls
