#!/bin/bash

MY_PRO_DIR="/home/junkaizhan/workspace/ftnoc"

# simulate pro
cd $MY_PRO_DIR/synopsys_env/vcs_gate
rm -rf c* si* l* u* lo*
python netlistInsertLogModuleParameter.py

echo "PP20"
# Introduction
echo "You are running a script to generate simulation files using the vcs-tool from synopsis"

LIBRARY_INDICATOR_V="-v"

MY_CODE_DIR=$2
MY_GATE_DIR=$4
MY_LIB_DIR=$6
MY_TESTBENCH_DIR=$8
SEARCH_PATH=${10}

OPTIONS="-full64 -PP -debug_all +lint=all +v2k -notice -ntb_opts tb_timescale=1ns/10ps -V -sverilog +sdfverbose +incdir+$SEARCH_PATH"
echo "vcs compile options: \"$OPTIONS\""

MACROFILE="$MY_CODE_DIR/parameters.v"
echo "Macro files: \"$MACROFILE\""

LIBRARY_FILES="$LIBRARY_INDICATOR_V $MY_LIB_DIR/tcbn65gplus.v"
echo "Library files: \"$LIBRARY_FILES\""

# This needs a the sdf command in the corresponding the verilog file
# Like this e.g. "initial $sdf_annotate("../dcout_files/m_Assembled_NoC.sdf");"
SDF_FILES="+compsdf"

MAIN_FILES="$SDF_FILES $MY_GATE_DIR/m_Assembled_Router_test.v "
echo "Main files: \"$MAIN_FILES\""

T="$MY_CODE_DIR/m_LogModule.v"
U="$MY_CODE_DIR/m_Assembled_NoC.v"

TESTBENCH_FILES="$MY_CODE_DIR/m_Assembled_NoC_w_RandomFaultInj_tb.sv $MY_CODE_DIR/m_PacketInjector.sv $MY_CODE_DIR/m_RandomInjector.v $MY_CODE_DIR/m_ClockGenerator.v"
echo "Testbench files: \"$TESTBENCH_FILES\""  

sleep 5

# Running VCS
# command-line: vcs source_files [source_or_object_files] options
vcs $OPTIONS ${12} $TESTBENCH_FILES $MACROFILE $LIBRARY_FILES $T $MAIN_FILES $U

echo 
echo 
echo "Script finished" 
echo


# 
echo "============== vcs finish ======================="

echo "PP80"
# generate simulation log
./simv >> log
echo "PP90"
# sleep 20

# translate simulation log to trace 
python script_logPrep_insertCycle.py log

echo "PP100"
# go home
cd $CURRENT_PWD
