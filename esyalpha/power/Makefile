# this file is for local test purpose only

CC     = gcc

DEFS   = -DPOWER_TEST -DPARM_TECH_POINT=045 -DVdd=1 -DPARM_Freq=2.5e9

CFLAGS = -O0 -g -w -Wall  -Wno-unused -I.  $(DEFS)


AR     = ar
RANLIB = ranlib

ARRAY_MODEL  = SIM_power_array_l.c SIM_power_array_m.c SIM_power_cam.c
ROUTER_MODEL = SIM_power_router.c SIM_router_power.c router.c
ALU_MODEL    = SIM_power_ALU.c
OTHER_MODEL  = SIM_power_misc.c SIM_power_permu.c
MISC_SRC     = SIM_power_util.c SIM_power_time.c
SRCS         = $(ARRAY_MODEL) $(ROUTER_MODEL) $(ALU_MODEL) $(OTHER_MODEL) \
               $(MISC_SRC)


OBJS      = $(SRCS:.c=.o)
LIB       = libpower.a

all:	$(LIB)

$(LIB): $(OBJS)
	$(AR) cr $@ $?
	$(RANLIB) $@



clean:
	$(RM) *.o *.a 

depend:
	makedepend -I. -I../library -Y $(ALL_SRCS) 2>/dev/null

# DO NOT DELETE

SIM_power_array_l.o: LIB_defines.h SIM_power.h
SIM_power_array_l.o: SIM_power_array_internal.h SIM_power_array.h
SIM_power_array_m.o: LIB_defines.h SIM_power.h
SIM_power_array_m.o: SIM_power_array_internal.h SIM_power_array.h
SIM_power_cam.o: SIM_power.h LIB_defines.h
SIM_power_cam.o: SIM_power_array_internal.h SIM_power_array.h
SIM_power_router.o: LIB_defines.h SIM_power.h
SIM_power_router.o: SIM_power_array.h SIM_power_array_internal.h
SIM_power_router.o: SIM_power_misc.h SIM_power_misc_internal.h
SIM_power_router.o: SIM_power_router.h SIM_power_router_internal.h
SIM_power_ALU.o: SIM_power_ALU.h LIB_defines.h
SIM_power_ALU.o: SIM_power.h
SIM_power_misc.o: LIB_defines.h SIM_power.h
SIM_power_misc.o: SIM_power_misc_internal.h SIM_power_misc.h
SIM_power_permu.o: SIM_power_permu.h LIB_defines.h
SIM_power_permu.o: SIM_power.h
SIM_power_util.o: LIB_defines.h SIM_power.h
SIM_power_time.o: SIM_power.h LIB_defines.h
SIM_power_time.o: SIM_power_array_internal.h
test_router.o: SIM_power.h LIB_defines.h
test_router.o: SIM_power_router.h SIM_power_router_internal.h
test_router.o: SIM_power_misc.h SIM_power_misc_internal.h SIM_power_array.h
test_router.o: SIM_power_array_internal.h SIM_router_power.h SIM_port.h
test_permu.o: SIM_power.h LIB_defines.h
test_permu.o: SIM_power_permu.h
test_alu.o: SIM_power_ALU.h LIB_defines.h SIM_power.h
dump_para.o: SIM_power.h LIB_defines.h
SIM_router_power.o: SIM_port.h SIM_power.h
SIM_router_power.o: LIB_defines.h SIM_power_array.h
SIM_router_power.o: SIM_power_array_internal.h SIM_power_router.h
SIM_router_power.o: SIM_power_router_internal.h SIM_power_misc.h
SIM_router_power.o: SIM_power_misc_internal.h
