#!/bin/bash

source ./env_config.sh

MY_PRO_DIR="/home/junkaizhan/workspace/ftnoc"

# simulate pro
cd $MY_PRO_DIR/synopsys_env/vcs_front
rm -rf c* si* l* u* lo*

echo "PP20"

# Introduction
echo "You are running a script to generate simulation files using the vcs-tool from synopsis"

# Loading parameters - the loaded files are from the NoC Project of the TU Wien
LIBRARY_INDICATOR_V="-v"
# SystemVerilog functionality trigger - I expected that -sverilog is also a library file indicator but it doesn't seem so
#SV=""
# jk
MY_CODE_DIR=$2
MY_LIB_DIR=$4
MY_TESTBENCH_DIR=$6
SEARCH_PATH=$8

OPTIONS="-full64 -PP -debug_all +lint=all +v2k -notice -ntb_opts -V -sverilog +sdfverbose +incdir+$SEARCH_PATH"
echo "vcs compile options: \"$OPTIONS\""

MACROFILE="$MY_CODE_DIR/parameters.v"
echo "Macro files: \"$MACROFILE\""

LIBRARY_FILES="$LIBRARY_INDICATOR_V $MY_LIB_DIR/tcbn65gplus.v"
echo "Library files: \"$LIBRARY_FILES\""

# This needs a the sdf command in the corresponding the verilog file
# Like this e.g. "initial $sdf_annotate("../dcout_files/m_Assembled_NoC.sdf");"
SDF_FILES="+compsdf"

MAIN_FILES="$SDF_FILES $MY_CODE_DIR/m_Assembled_NoC.v "
echo "Main files: \"$MAIN_FILES\""

A="$MY_CODE_DIR/m_D_FF_mod.v"
B="$MY_CODE_DIR/m_D_FF_sync.v"
C="$MY_CODE_DIR/m_Demux_1in2out.v"
D="$MY_CODE_DIR/m_Mux_2in1out.v"
E="$MY_CODE_DIR/m_Mux_NoEncoding.v"
F="$MY_CODE_DIR/m_Buffer.v"
G="$MY_CODE_DIR/m_RoutingCalculation.v"
H="$MY_CODE_DIR/m_StateMachine.v"
I="$MY_CODE_DIR/m_MatrixArbiter_subUnit.v"
J="$MY_CODE_DIR/m_RR_MatrixArbiter.v"
K="$MY_CODE_DIR/m_RR_EvaluationUnit.v"
L="$MY_CODE_DIR/m_MatrixArbiter.v"
M="$MY_CODE_DIR/m_EvaluationUnit.v"
N="$MY_CODE_DIR/m_SwitchAllocation.v"
O="$MY_CODE_DIR/m_Switch_7in7out.v"
P="$MY_CODE_DIR/m_Switch_Feedback_7in7out.v"
Q="$MY_CODE_DIR/m_Crossbar.v"
R="$MY_CODE_DIR/m_Assembled_Channel.v"
S="$MY_CODE_DIR/m_Assembled_Router.v"
T="$MY_CODE_DIR/m_LogModule.v"
U="$MY_CODE_DIR/m_Switch_Data.v"

TESTBENCH_FILES="$MY_TESTBENCH_DIR/m_Assembled_NoC_w_RandomFaultInj_tb.sv $MY_TESTBENCH_DIR/m_PacketInjector.sv $MY_TESTBENCH_DIR/m_RandomInjector.v $MY_TESTBENCH_DIR/m_ClockGenerator.v"
echo "Testbench files: \"$TESTBENCH_FILES\""  

# echo "****"
# echo "***"
# echo "**"
# echo "* DID YOU ADD \"initial $sdf_annotate(\"./m_Assembled_NoC.sdf\");\" to m_Assembled_NoC.v"
# echo "**"
# echo "***"
# echo "****"

sleep 5

# Running VCS
# command-line: vcs source_files [source_or_object_files] options
vcs ${10} $OPTIONS $TESTBENCH_FILES $MACROFILE $LIBRARY_FILES $A $B $C $D $E $F $G $H $I $J $K $L $M $N $P $Q $R $S $T $U $MAIN_FILES 

echo 
echo 
echo "Script finished" 
echo

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
