
# SYNOPSYS TOOLS

export SYNOPSYS_HOME=~/software/synopsys

# lmgrd
export PATH="$SYNOPSYS_HOME/scl_11_09/amd64/bin:$PATH"
# start synopsys license using lmgrd
alias lmli2="lmgrd -c $SYNOPSYS_HOME/license/synopsys_new.dat -l ~/Documents/syn_lic.log"

export VCS_ARCH_OVERRIDE=linux

export PATH="$SYNOPSYS_HOME/VCS_2012_09/bin:"$PATH
export PATH="$SYNOPSYS_HOME/VCS_2012_09/gui/dve/bin:"$PATH
export PATH="$SYNOPSYS_HOME/DC_2015_06/bin:"$PATH
export PATH="$SYNOPSYS_HOME/PT_2013_06/bin:"$PATH
export PATH="$SYNOPSYS_HOME/ICC_2012_06/bin:"$PATH
export PATH="$SYNOPSYS_HOME/FM_2013_03/bin:"$PATH
export PATH="$SYNOPSYS_HOME/STARRC_2012_12/amd64_starrc/bin:"$PATH

export SYNOPSYS="$SYNOPSYS_HOME"
export LM_LICENSE_FILE="$SYNOPSYS_HOME/license/synopsys_new.dat"
export VCS_HOME="$SYNOPSYS_HOME/VCS_2012_09"
export DC_HOMOE="$SYNOPSYS_HOME/DC_2015_06"
export PT_HOME="$SYNOPSYS_HOME/PT_2013_06"
export ICC_HOME="$SYNOPSYS_HOME/ICC_2012_06"
export FM_HOME="$SYNOPSYS_HOME/FM_2013_03"
export STAR_RCXT_HOME_DIR="$SYNOPSYS_HOME/STARRC_2012_12/amd64_starrc"

# SNPSLMD_LICENSE_FILE
export SNPSLMD_LICENSE_FILE=27000@jkubuntu
# export SNPSLMD_LICENSE_FILE=$SYNOPSYS_HOME/license/synopsys.dat

# alias vcs="vcs -full64"
alias dve="dve -full64"
alias dc="dc_shell"
alias dv="design_visioin"
alias pt="primetime"
alias icc="icc_shell"
alias fm="fm_shell"
alias rcxt="StarXtract"

CURRENT_PWD=`pwd`
