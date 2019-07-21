
include ../../Makefile.include

# IBM SA-27 technology (InfiniBand)
#DEFS   = -DPOWER_TEST -DPARM_TECH_POINT=18 -DVdd=1.8 -DPARM_Freq=2.5e8
# IBM SA-27E technology (RAW)
#DEFS   = -DPOWER_TEST -DPARM_TECH_POINT=11 -DVdd=1.8 -DPARM_Freq=3e8
# IBM Cu-11 technology
#DEFS   = -DPOWER_TEST -DPARM_TECH_POINT=11 -DVdd=1.2 -DPARM_Freq=2.5e8

# for general testing
DEFS   := -DPOWER_TEST -DPARM_TECH_POINT=045 -DVdd=1 -DPARM_Freq=2.5e9

CFLAGS += -w -I. $(DEFS)

MODULE := esynet$(X)orion

SUBMODULE := 

include ../../Makefile.rule
