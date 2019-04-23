# general test
#./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 1 -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 \
#	-link_length 10000 -routing_alg Rescuer -random_seed 1 -sim_length 20000 -simulation_period 1 \
#	-traffic_rule 0 -traffic_pir 0.1 -packet_size 5 \
#	-injected_packet 10000 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 \
#	-event_trace_enable -event_trace_buffer_size 10000 -event_trace_file_name /home/jwang/VisualNoC/example/rescuer \

# data path faults test
#./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 1 -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 \
#	-fault_inject_enable -fault_inject_file_name /home/jwang/VisualNoC/example/faultcfg \ 
#	-link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 20000 -simulation_period 1 \
#	-traffic_rule 0 -traffic_pir 0.1 -packet_size 5 \
#	-injected_packet 10000 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 \
#	-event_trace_enable -event_trace_buffer_size 10000 -event_trace_file_name /home/jwang/VisualNoC/example/probmodel \

# data path fault, under different pir
#rm parafile.txt
#for pir in $(seq 0.02 0.02 0.2); do
#echo ./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 1 -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 -fault_inject_enable -fault_inject_file_name /home/jwang/VisualNoC/example/faultcfg -link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 20000 -simulation_period 1 -traffic_rule 0 -traffic_pir $pir -packet_size 5 -injected_packet 10000 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt
#done
#../qtparallel/qtparallel -pc_num_avail 1 -arg_file_path parafile.txt -out_file_path data_path_pir.xls
#rm parafile.txt

# data path fault, under different fault injection probability
#rm parafile.txt
#for l2fprob in $(seq 0.0 0.00005 0.001); do
#	l2lprob=$(echo "1-$l2fprob"|bc)
#	f2lprob=0.9
#	f2fprob=0.1
#	echo ./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 1 -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 -fault_inject_enable -fault_inject_file_name /home/jwang/VisualNoC/example/faultcfg -fault_inject_inline 4 -1 -1 -1 -1 0 1 e $l2lprob $l2fprob $f2lprob $f2fprob e 2 -1 -1 -1 -1 0 1 e $l2lprob $l2fprob $f2lprob $f2fprob e -link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 20000 -simulation_period 1 -traffic_rule 0 -traffic_pir 0.01 -packet_size 5 -injected_packet 10000 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt
#done
#../qtparallel/qtparallel -pc_num_avail 1 -arg_file_path parafile.txt -out_file_path data_path_without_ecc_fips.xls
#rm parafile.txt

#echo ./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 1 -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 -fault_inject_enable -fault_inject_file_name /home/jwang/VisualNoC/example/faultcfg -fault_inject_inline 4 -1 -1 -1 -1 0 1 e $l2lprob $l2fprob $f2lprob $f2fprob e 2 -1 -1 -1 -1 0 1 e $l2lprob $l2fprob $f2lprob $f2fprob e -link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 20000 -simulation_period 1 -traffic_rule 0 -traffic_pir 0.01 -packet_size 5 -injected_packet 10000 -ecc_enable -ecc_type 0 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt

# data path fault, under different fault injection probability
#rm parafile.txt
#for l2fprob in $(seq 0.0 0.00005 0.001); do
#	l2lprob=$(echo "1-$l2fprob"|bc)
#	f2lprob=0.9
#	f2fprob=0.1
#	echo ./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 1 -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 -fault_inject_enable -fault_inject_file_name /home/jwang/VisualNoC/example/faultcfg -fault_inject_inline 4 -1 -1 -1 -1 0 1 e $l2lprob $l2fprob $f2lprob $f2fprob e 2 -1 -1 -1 -1 0 1 e $l2lprob $l2fprob $f2lprob $f2fprob e -link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 20000 -simulation_period 1 -traffic_rule 0 -traffic_pir 0.01 -packet_size 5 -injected_packet 10000 -ecc_enable - -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt
#done
#../qtparallel/qtparallel -pc_num_avail 1 -arg_file_path parafile.txt -out_file_path data_path_without_ecc_fips.xls
#rm parafile.txt

#./esynet -router_num 16 -array_size 4 4 -phy_number 5 -vc_number 4 -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 \
#	-link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 10000 -simulation_period 1 \
#	-traffic_rule 0 -traffic_pir 0.1 -packet_size 5 \
#	-injected_packet -1 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 \
#	-event_trace_enable -event_trace_buffer_size 10000 -event_trace_file_name /home/jwang/VisualNoC/example/probmodel

# 2 vc or 4 vc, packet size 1, 3, or 5
#rm parafile.txt
#for vc in 2 4; do
#  for pir in $(seq 0.06 0.06 1.2); do
#    echo ./esynet -router_num 16 -array_size 4 4 -phy_number 5 -vc_number $vc -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 -link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 10000 -simulation_period 1 -traffic_rule 0 -traffic_pir $pir -packet_size 1 -injected_packet -1 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt
#  done
#done
#for vc in 2 4; do
#  for pir in $(seq 0.02 0.02 0.4); do
#    echo ./esynet -router_num 16 -array_size 4 4 -phy_number 5 -vc_number $vc -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 -link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 10000 -simulation_period 1 -traffic_rule 0 -traffic_pir $pir -packet_size 3 -injected_packet -1 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt
#  done
#done
#for vc in 2 4; do
#  for pir in $(seq 0.012 0.012 0.24); do
#    echo ./esynet -router_num 16 -array_size 4 4 -phy_number 5 -vc_number $vc -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 -link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 10000 -simulation_period 1 -traffic_rule 0 -traffic_pir $pir -packet_size 5 -injected_packet -1 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt
#  done
#done
#for vc in 2 4; do
#  for pir in $(seq 0.03 0.03 0.6); do
#    echo ./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number $vc -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 -link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 10000 -simulation_period 1 -traffic_rule 0 -traffic_pir $pir -packet_size 1 -injected_packet -1 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt
#  done
#done
#for vc in 2 4; do
#  for pir in $(seq 0.01 0.01 0.2); do
#    echo ./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number $vc -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 -link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 10000 -simulation_period 1 -traffic_rule 0 -traffic_pir $pir -packet_size 3 -injected_packet -1 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt
#  done
#done
#for vc in 2 4; do
#  for pir in $(seq 0.006 0.006 0.12); do
#    echo ./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number $vc -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 -link_length 1000 -routing_alg 0 -random_seed 1 -sim_length 10000 -simulation_period 1 -traffic_rule 0 -traffic_pir $pir -packet_size 5 -injected_packet -1 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt
#  done
#done
#../qtparallel/qtparallel -pc_num_avail 1 -arg_file_path parafile.txt -out_file_path latency_4or8_2or4.xls
#rm parafile.txt

# orginzation location of ecc
#rm parafile.txt
#eccnameu="HM74"
#eccnamel="hm74"
#plf=0.0001
#pfl=0.9
#linkarea=10
#routerarea=30
#commenpara="echo ./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 1 -in_buffer_size 12 -out_buffer_size 12 -flit_size 5 -data_path_width 32 -fault_inject_enable -fault_inject_file_name ../example/faultcfg_${eccnamel}_${plf}_${pfl}_${linkarea}_${routerarea} -link_length 1000 -routing_alg XY -random_seed 1 -sim_length 10000 -simulation_period 1 -traffic_rule 0 -traffic_pir 0.05 -packet_size 5 -injected_packet 10000 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 -ecc_enable"
# H2H
#$commenpara -ecc_name $eccnameu -ecc_strategy H2H -ecc_space 0 >> parafile.txt
# Cross
#for space in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16; do
#  $commenpara -ecc_name $eccnameu -ecc_strategy Cross -ecc_space $space >> parafile.txt
#done
# Slope
#for space in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16; do
#  $commenpara -ecc_name $eccnameu -ecc_strategy Slope -ecc_space $space >> parafile.txt
#done
# Square
#for space in 1 2 3 4 5 6 7 8; do
#  $commenpara -ecc_name $eccnameu -ecc_strategy Square -ecc_space $space >> parafile.txt
#done
# Count
#for space in 3; do
#  $commenpara -ecc_name $eccnameu -ecc_strategy Counter -ecc_space $space >> parafile.txt
#done
# E2E
#$commenpara -ecc_name $eccnameu -ecc_strategy E2E -ecc_space 0 >> parafile.txt
#../qtparallel/qtparallel -pc_num_avail 1 -arg_file_path parafile.txt -out_file_path latency_4or8_2or4.xls
#rm parafile.txt

# ./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 1 -in_buffer_size 12 -out_buffer_size 12 -flit_size 5 -data_path_width 32 -fault_inject_enable -fault_inject_file_name ../example/faultcfg_hm74_0.0001_0.9_10_30 -link_length 1000 -routing_alg XY -random_seed 1 -sim_length 10000 -simulation_period 1 -traffic_rule 0 -traffic_pir 0.05 -packet_size 5 -injected_packet 10000 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 -ecc_enable -ecc_name HM74 -ecc_strategy Cross -ecc_space 1

#./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 1 -in_buffer_size 12 -out_buffer_size 12 -flit_size 5 -data_path_width 32 -fault_inject_enable -fault_inject_file_name ../example/faultcfg_hm74_0.0001_0.9_10_30 -link_length 1000 -routing_alg XY -random_seed 1 -sim_length 10000 -simulation_period 1 -traffic_rule 0 -traffic_pir 0.05 -packet_size 5 -injected_packet 10000 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 -ecc_enable -ecc_name HM74 -ecc_strategy Counter -ecc_space 1

# latency vs size
#rm parafile.txt
#for size in 3 4 5 6; do
#  for pir in $(seq 0.05 0.05 1.0); do
#    num=$(echo "$size*$size"|bc)
#    echo ./esynet -router_num $num -array_size $size $size -phy_number 5 -vc_number 1 -in_buffer_size 5 -out_buffer_size 5 -flit_size 1 -data_path_width 32 -link_length 10000 -routing_alg XY -random_seed 1 -sim_length 50000 -simulation_period 1 -traffic_rule Uniform -traffic_pir $pir -packet_size 1 -injected_packet -1 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt
#  done
#done
#../qtparallel/qtparallel -pc_num_avail 1 -arg_file_path parafile.txt -out_file_path latency_vs_size.xls
#rm parafile.txt

# one fault test
#rm parafile.txt
#for r in $(seq 0 1 63); do
#  echo ./esynet -router_num 64 -array_size 8 8 -phy_number 7 -vc_number 2 -in_buffer_size 12 -out_buffer_size 12 -flit_size 1 -data_path_width 32 -link_length 1000 -routing_alg NonBlocking -router_cfg_enable -router_cfg_file_name /home/jwang/VisualNoC-develop/example/mesh_nonblocking.xml -random_seed 1 -sim_length 10000 -simulation_period 1 -traffic_rule Uniform -traffic_pir 0.07 -packet_size 5 -fault_inject_enable -fault_inject_file_name /home/jwang/VisualNoC-develop/example/faultinject -fault_inject_inline 0 $r -1 -1 -1 1 e 1 e -injected_packet 10000 -warmup_packet 0 -latency_measure_packet -1 -throughput_measure_packet -1 >> parafile.txt
#done
#../qtparallel/qtparallel -pc_num_avail 2 -arg_file_path parafile.txt -out_file_path ../example/nonblocking_one_fault.xls
#rm parafile.text

networkcfg="./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 2 -in_buffer_size 12 -out_buffer_size 12 -data_path_width 32 -link_length 1000 -routing_alg DyXY -random_seed 1 -simulation_period 1 -ni_buffer_size 1 -ni_read_delay 0 -sim_length 10000 -traffic_rule Uniform -traffic_pir 0.01 -packet_size 5 -injected_packet 3000 -fault_inject_enable -fault_inject_file_name ../example/faultinject -fault_inject_inline 2 -1 -1 -1 -1 0 1 e 0.99 0.01 0.9 0.1 e"
ecccfg="-ecc_enable -ecc_name HM128 -ecc_strategy H2H"
rtcfg="-e2e_retrans_enable -e2e_retrans_timer_max 1000"
swcfg="-sw_enable -sw_capacity 16 2"
ftrcfg="-routing_alg HiPFaR"
#$networkcfg
#$networkcfg $ecccfg
$networkcfg $ecccfg $rtcfg
#$networkcfg $ecccfg $ftrcfg
#$networkcfg $ecccfg $ftrcfg $rtcfg
#$networkcfg $ecccfg $swcfg
#$networkcfg $ecccfg $swcfg $rtcfg
#$networkcfg $ecccfg $swcfg $ftrcfg
#$networkcfg $ecccfg $swcfg $ftrcfg $rtcfg

./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 2 -in_buffer_size 12 -out_buffer_size 12 -data_path_width 32 -link_length 1000 -traffic_rule ${bench} -traffic_pir 0.05 -routing_alg DyXY -random_seed 1 -sim_length 10000 -simulation_period 1 -event_trace_enable -event_trace_file_name ../example/event_${bench} -event_trace_file_text_enable

