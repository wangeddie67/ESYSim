
for p in $(seq 1 0.1 1); do
	{
		./esymap \
		-router_num	64 \
		-random_seed	1  \
		-sim_length	10000 \
		-app_size_min  4\
		-app_size_max  20\
		-packet_size_min 5\
		-packet_size_max 5\
		-period_min  2000\
		-period_max  4000\
		-period_exec_times_min 2\
		-period_exec_times_max 10\
		-mapping_algs cona\
		-tgff_file_name ./TGFF/simple.tgff\
		-result_file_name ./result.txt\
		-task_order_type cona\
		-pe_rate 5 \
		-app_inject_rate 1000\
		-system_usage_rate $p
	} # &
done

