#!/bin/bash

declare -a "esyarch_dir"
esyarch_dir=("./records" "./records/inststream" "./records/pc2line" "./records/power" "./records/progout" "./records/statistics" 
	"./records/power/power_xml")

function esyarch_mkdir {
	for dir in ${esyarch_dir[@]}
	do
		if [ ! -d ${dir} ]; then
			mkdir ${dir}
		fi
	done
}

esyarch_mkdir
./sim-outorder $1

