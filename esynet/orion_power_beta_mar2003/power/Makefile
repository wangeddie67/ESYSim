# this file is for local test purpose only

#Makefile for popnet
include ../../../Makefile.include

# IBM SA-27 technology (InfiniBand)
#DEFS   = -DPOWER_TEST -DPARM_TECH_POINT=18 -DVdd=1.8 -DPARM_Freq=2.5e8
# IBM SA-27E technology (RAW)
#DEFS   = -DPOWER_TEST -DPARM_TECH_POINT=11 -DVdd=1.8 -DPARM_Freq=3e8
# IBM Cu-11 technology
#DEFS   = -DPOWER_TEST -DPARM_TECH_POINT=11 -DVdd=1.2 -DPARM_Freq=2.5e8

# for general testing
DEFS   = -DPOWER_TEST -DPARM_TECH_POINT=045 -DVdd=1 -DPARM_Freq=2.5e9

CFLAGS += -I. -I../library $(DEFS)
#CFLAGS = -I. -I../library $(DEFS) -O3 -Wno-deprecated -static
#CFLAGS = -I. -I../library $(DEFS) -g

ARRAY_MODEL  = SIM_power_array_l.c \
	SIM_power_array_m.c \
	SIM_power_cam.c
ROUTER_MODEL = SIM_power_router.c \
	SIM_router_power.c 

	#router.c
ALU_MODEL    = SIM_power_ALU.c
OTHER_MODEL  = SIM_power_misc.c \
	SIM_power_permu.c
MISC_SRC     = SIM_power_util.c \
	SIM_power_time.c
SRCS         = $(ARRAY_MODEL) \
	$(ROUTER_MODEL) \
	$(ALU_MODEL) \
	$(OTHER_MODEL) \
	$(MISC_SRC)

HEADERS   = SIM_power.h \
	SIM_power_ALU.h \
	SIM_power_array.h \
	SIM_power_array_internal.h \
	SIM_power_misc.h \
	SIM_power_misc_internal.h \
	SIM_power_permu.h \
	SIM_power_router.h \
	SIM_power_router_internal.h \
	SIM_router_power.h

OBJS      = SIM_power_array_l.$(OEXT) \
	SIM_power_array_m.$(OEXT) \
	SIM_power_cam.$(OEXT) \
	SIM_power_router.$(OEXT) \
	SIM_router_power.$(OEXT) \
	SIM_power_ALU.$(OEXT) \
	SIM_power_misc.$(OEXT) \
	SIM_power_permu.$(OEXT) \
	SIM_power_util.$(OEXT) \
	SIM_power_time.$(OEXT)

LIB       = libpower.$(LEXT)

all:	$(LIB)

$(LIB): $(OBJS)
	$(AR) $(AROPT)$@ $(OBJS)
	$(RANLIB) $@

TEST_SRCS   = test_router.c \
	test_permu.c \
	test_alu.c \
	dump_para.c
TEST_EXEC   = $(TEST_SRCS:.c=)
MODULE_SRCS = SIM_router_power.c
ALL_SRCS    = $(SRCS) \
	$(TEST_SRCS) \
	$(MODULE_SRCS)
ALL_OBJS    = $(ALL_SRCS:.c=.o)

install: all
	mkdir -p $(LIBERTY)$(X)include$(X)library$(X)power
	cp $(HEADERS) $(LIBERTY)$(X)include$(X)library$(X)power
	mkdir -p $(LIBERTY)$(X)lib
	cp $(LIB) $(LIBERTY)$(X)lib

test_router: test_router.o SIM_router_power.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $< -lm

test_permu: test_permu.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $< -lm

test_alu: test_alu.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $< -lm

dump_para: dump_para.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $< -lm

clean:
	$(RM) *.o *.obj $(LIB) $(TEST_EXEC) *~ *.bak core

depend:
	makedepend -I. -I../library -Y $(ALL_SRCS) 2>/dev/null
# DO NOT DELETE

SIM_power_array_l.$(OEXT): ../library/library_LIB/LIB_defines.h SIM_power.h
SIM_power_array_l.$(OEXT): SIM_power_array_internal.h SIM_power_array.h
SIM_power_array_m.$(OEXT): ../library/library_LIB/LIB_defines.h SIM_power.h
SIM_power_array_m.$(OEXT): SIM_power_array_internal.h SIM_power_array.h
SIM_power_cam.$(OEXT): SIM_power.h ../library/library_LIB/LIB_defines.h
SIM_power_cam.$(OEXT): SIM_power_array_internal.h SIM_power_array.h
SIM_power_router.$(OEXT): ../library/library_LIB/LIB_defines.h SIM_power.h
SIM_power_router.$(OEXT): SIM_power_array.h SIM_power_array_internal.h
SIM_power_router.$(OEXT): SIM_power_misc.h SIM_power_misc_internal.h
SIM_power_router.$(OEXT): SIM_power_router.h SIM_power_router_internal.h
SIM_power_ALU.$(OEXT): SIM_power_ALU.h ../library/library_LIB/LIB_defines.h
SIM_power_ALU.$(OEXT): SIM_power.h
SIM_power_misc.$(OEXT): ../library/library_LIB/LIB_defines.h SIM_power.h
SIM_power_misc.$(OEXT): SIM_power_misc_internal.h SIM_power_misc.h
SIM_power_permu.$(OEXT): SIM_power_permu.h ../library/library_LIB/LIB_defines.h
SIM_power_permu.$(OEXT): SIM_power.h
SIM_power_util.$(OEXT): ../library/library_LIB/LIB_defines.h SIM_power.h
SIM_power_time.$(OEXT): SIM_power.h ../library/library_LIB/LIB_defines.h
SIM_power_time.$(OEXT): SIM_power_array_internal.h
test_router.$(OEXT): SIM_power.h ../library/library_LIB/LIB_defines.h
test_router.$(OEXT): SIM_power_router.h SIM_power_router_internal.h
test_router.$(OEXT): SIM_power_misc.h SIM_power_misc_internal.h SIM_power_array.h
test_router.$(OEXT): SIM_power_array_internal.h SIM_router_power.h SIM_port.h
test_permu.$(OEXT): SIM_power.h ../library/library_LIB/LIB_defines.h
test_permu.$(OEXT): SIM_power_permu.h
test_alu.$(OEXT): SIM_power_ALU.h ../library/library_LIB/LIB_defines.h SIM_power.h
dump_para.$(OEXT): SIM_power.h ../library/library_LIB/LIB_defines.h
SIM_router_power.$(OEXT): SIM_port.h SIM_power.h
SIM_router_power.$(OEXT): ../library/library_LIB/LIB_defines.h SIM_power_array.h
SIM_router_power.$(OEXT): SIM_power_array_internal.h SIM_power_router.h
SIM_router_power.$(OEXT): SIM_power_router_internal.h SIM_power_misc.h
SIM_router_power.$(OEXT): SIM_power_misc_internal.h
