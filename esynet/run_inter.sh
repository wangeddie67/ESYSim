networkcfg="./esynet -router_num 64 -array_size 8 8 -phy_number 5 -vc_number 2 -in_buffer_size 12 -out_buffer_size 12 -data_path_width 32 -link_length 1000 -routing_alg DyXY -simulation_period 1 -ni_buffer_size 1 -ni_read_delay 0 -sim_length 50000 -traffic_rule Uniform -traffic_pir 0.01 -packet_size 5 -injected_packet 30000 -fault_inject_enable -fault_inject_file_name ../example/faultinject" 
ecccfg="-ecc_enable -ecc_name HM128 -ecc_strategy H2H"
rtcfg="-e2e_retrans_enable -e2e_retrans_timer_max 300"
swcfg="-sw_enable -sw_capacity 16 1"
ftrcfg="-routing_alg HiPFaR"

#rm parafile.txt

#for l2fprob in $(seq 0.0 0.000000004 0.00000008); do
#	l2lprob=$(echo "1-$l2fprob"|bc)
#	l2fprob2=$(echo "scale=9;$l2fprob/2.0"|bc)
# no ft
#	echo $networkcfg "-fault_inject_inline 2 -1 -1 -1 -1 0 0 1 e $l2lprob $l2fprob2 $l2fprob2 0.0001 0.49995 0.49995 0.0001 0.49995 0.49995 e -random_seed 1" >> parafile.txt
#done

#for l2fprob in $(seq 0.0 0.00000005 0.000001); do
l2fprob=0.0000006
l2lprob=$(echo "1-$l2fprob"|bc)
l2fprob2=$(echo "scale=9;$l2fprob/2.0"|bc)
r=1
# ecc
#	echo $networkcfg "-fault_inject_inline 2 -1 -1 -1 -1 0 0 1 e $l2lprob $l2fprob2 $l2fprob2 0.0001 0.49995 0.49995 0.0001 0.49995 0.49995 e -random_seed $r" $ecccfg >> parafile.txt
# ecc rt
#	echo $networkcfg "-fault_inject_inline 2 -1 -1 -1 -1 0 0 1 e $l2lprob $l2fprob2 $l2fprob2 0.0001 0.49995 0.49995 0.0001 0.49995 0.49995 e -random_seed $r" $ecccfg $rtcfg >> parafile.txt
# ecc ftr
#	echo $networkcfg "-fault_inject_inline 2 -1 -1 -1 -1 0 0 1 e $l2lprob $l2fprob2 $l2fprob2 0.0001 0.49995 0.49995 0.0001 0.49995 0.49995 e -random_seed $r" $ecccfg $ftrcfg >> parafile.txt
# ecc ftr rt
$networkcfg -fault_inject_inline 2 -1 -1 -1 -1 0 0 1 e $l2lprob $l2fprob2 $l2fprob2 0.0001 0.49995 0.49995 0.0001 0.49995 0.49995 e -random_seed $r $ecccfg $ftrcfg $rtcfg
# ecc sw
#	echo $networkcfg "-fault_inject_inline 2 -1 -1 -1 -1 0 0 1 e $l2lprob $l2fprob2 $l2fprob2 0.0001 0.49995 0.49995 0.0001 0.49995 0.49995 e -random_seed $r" $ecccfg $swcfg >> parafile.txt
# ecc sw rt
#	echo $networkcfg "-fault_inject_inline 2 -1 -1 -1 -1 0 0 1 e $l2lprob $l2fprob2 $l2fprob2 0.0001 0.49995 0.49995 0.0001 0.49995 0.49995 e -random_seed $r" $ecccfg $swcfg $rtcfg >> parafile.txt
# ecc sw ftr
#	echo $networkcfg "-fault_inject_inline 2 -1 -1 -1 -1 0 0 1 e $l2lprob $l2fprob2 $l2fprob2 0.0001 0.49995 0.49995 0.0001 0.49995 0.49995 e -random_seed $r" $ecccfg $swcfg $ftrcfg >> parafile.txt
# ecc sw ftr rt
#	echo $networkcfg "-fault_inject_inline 2 -1 -1 -1 -1 0 0 1 e $l2lprob $l2fprob2 $l2fprob2 0.0001 0.49995 0.49995 0.0001 0.49995 0.49995 e -random_seed $r" $ecccfg $swcfg $ftrcfg $rtcfg >> #parafile.txt
#done
#done
#../qtparallel/qtparallel -pc_num_avail 70 -arg_file_path parafile.txt -out_file_path ../example/ft_inter.xls
#rm parafile.txt

