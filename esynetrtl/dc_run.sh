#!/bin/bash

# SYNOPSYS TOOLS

# lmgrd
export PATH="/home/junkaizhan/software/synopsys/scl_11_09/amd64/bin:"$PATH
# start synopsys license using lmgrd
alias lmli2="lmgrd -c /home/junkaizhan/software/synopsys/license/synopsys_new.dat -l ~/Documents/syn_lic.log"

export VCS_ARCH_OVERRIDE=linux

export PATH="/home/junkaizhan/software/synopsys/VCS_2012_09/bin:"$PATH
export PATH="/home/junkaizhan/software/synopsys/VCS_2012_09/gui/dve/bin:"$PATH
export PATH="/home/junkaizhan/software/synopsys/DC_2015_06/bin:"$PATH
export PATH="/home/junkaizhan/software/synopsys/PT_2013_06/bin:"$PATH
export PATH="/home/junkaizhan/software/synopsys/ICC_2012_06/bin:"$PATH
export PATH="/home/junkaizhan/software/synopsys/FM_2013_03/bin:"$PATH
export PATH="/home/junkaizhan/software/synopsys/STARRC_2012_12/amd64_starrc/bin:"$PATH

export SYNOPSYS="/home/junkaizhan/software/synopsys"
export LM_LICENSE_FILE="/home/junkaizhan/software/synopsys/license/synopsys_new.dat"
export VCS_HOME="/home/junkaizhan/software/synopsys/VCS_2012_09"
export DC_HOMOE="/home/junkaizhan/software/synopsys/DC_2015_06"
export PT_HOME="/home/junkaizhan/software/synopsys/PT_2013_06"
export ICC_HOME="/home/junkaizhan/software/synopsys/ICC_2012_06"
export FM_HOME="/home/junkaizhan/software/synopsys/FM_2013_03"
export STAR_RCXT_HOME_DIR="/home/junkaizhan/software/synopsys/STARRC_2012_12/amd64_starrc"

# SNPSLMD_LICENSE_FILE
export SNPSLMD_LICENSE_FILE=27000@jkubuntu
# export SNPSLMD_LICENSE_FILE=/home/junkaizhan/software/synopsys/license/synopsys.dat

# alias vcs="vcs -full64"
alias dve="dve -full64"
alias dc="dc_shell"
alias dv="design_visioin"
alias pt="primetime"
alias icc="icc_shell"
alias fm="fm_shell"
alias rcxt="StarXtract"

MY_PRO_DIR="/home/junkaizhan/workspace/ftnoc"

sleep 5
CURRENT_PWD=`pwd`

# cd $MY_PRO_DIR/chikun_gen
# cp $MY_PRO_DIR/chikun/src/* $MY_PRO_DIR/chikun
# cp $MY_PRO_DIR/chikun/tb/* $MY_PRO_DIR/chikun
# rm -rf $MY_PRO_DIR/chikun/src
# rm -rf $MY_PRO_DIR/chikun/tb

# dc_shell -gui
cd $MY_PRO_DIR/synopsys_env/dc_router
dc -f ~/ftnoc/scripts/dc_router.tcl

cd $CURRENT_PWD
