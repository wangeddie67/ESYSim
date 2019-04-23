
include ../Makefile.include

FFLAGS = -DDEBUG  -DMTA -DDEBUG1 -w -DREMOVE 
OFLAGS = -Wno-unused
MFLAGS = -DBYTES_LITTLE_ENDIAN -DWORDS_LITTLE_ENDIAN -DFAST_SRL -DFAST_SRA \
	-DPOWER_TEST

export ESYNETLINKED += -DESYNETINTERFACE

# common objects
OBJS =	main.$(OEXT) sim-outorder.$(OEXT) syscall.$(OEXT) memory.$(OEXT) \
	regs.$(OEXT) loader.$(OEXT) endian.$(OEXT) dlite.$(OEXT) \
	symbol.$(OEXT) eval.$(OEXT) options.$(OEXT) stats.$(OEXT) eio.$(OEXT) \
	range.$(OEXT) misc.$(OEXT) power.$(OEXT) context.$(OEXT) \
	cluster.$(OEXT) cmt.$(OEXT) machine.$(OEXT) bpred.$(OEXT) \
	cache.$(OEXT) ptrace.$(OEXT) resource.$(OEXT) cluster-opts.$(OEXT) \
	interconnect.$(OEXT) network.$(OEXT) mystats.$(OEXT) \
	temperature.$(OEXT) RCutil.$(OEXT) flp.$(OEXT) util.$(OEXT) \
	hotspot.$(OEXT) tokencoherence.$(OEXT) branchCorr.$(OEXT) \
	instManip.$(OEXT) skeleton_chk.$(OEXT) store2Load.$(OEXT) \
	indirectjump.$(OEXT) esynet.$(OEXT)

# network to buil : popnet relay network or free-space optical network
ESYNET_DIR = ..$(X)esynet
ESYNET_TARGET = $(ESYNET_DIR)$(X)esynet_lib
ESYNET_LIB = $(ESYNET_DIR)$(X)*.$(OEXT)

FSOI_DIR = .$(X)optical
FSOI_TARGET = $(FSOI_DIR)$(X)fsoi_lib
FSOI_LIB = $(FSOI_DIR)$(X)*.$(OEXT)

POWER_RELEASE = orion_power_beta_mar2003
PTARGET = power_model

CFLAGS += $(MFLAGS) $(FFLAGS) $(OFLAGS) \
	-I$(ESYNET_DIR)$(X)$(POWER_RELEASE)$(X)power \
	-I$(ESYNET_DIR)$(X)$(POWER_RELEASE)$(X)library \
	-I$(ESYNET_DIR) $(ALPHAFLAGS)

# all targets, NOTE: library ordering is important...
all sim-outorder: $(ESYNET_TARGET) $(FSOI_TARGET) $(OBJS) cacti exo
	$(CCPP) -o sim-outorder $(OBJS) $(ESYNET_LIB) $(FSOI_LIB) \
		libcacti/libcacti.$(LEXT) libexo/libexo.$(LEXT) \
		$(ESYNET_DIR)$(X)$(POWER_RELEASE)$(X)power/libpower.$(LEXT) \
		$(INTERFACE_LIB) $(TINYXML_LIB) -lm

sysprobe$(EEXT):	sysprobe.c
	$(CC) -g -w -o sysprobe$(EEXT) sysprobe.c
	@echo endian probe results: $(ENDIAN)
	@echo probe flags: $(MFLAGS)
	@echo probe libs: $(MLIBS)

$(ESYNET_TARGET):
	cd $(ESYNET_DIR); $(MAKE) interface

$(FSOI_TARGET):
	cd $(FSOI_DIR); $(MAKE)

exo libexo/libexo.$(LEXT):
	cd libexo $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "AR=$(AR)" "AROPT=$(AROPT)" \
		"RANLIB=$(RANLIB)" "CFLAGS=$(MFLAGS) $(FFLAGS) $(OFLAGS)" \
		"OEXT=$(OEXT)" "LEXT=$(LEXT)" "EEXT=$(EEXT)" "X=$(X)" \
		"RM=$(RM)" libexo.$(LEXT)

cacti libcacti/libcacti.$(LEXT):
	cd libcacti $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "AR=$(AR)" "AROPT=$(AROPT)" \
		"RANLIB=$(RANLIB)" "CFLAGS=$(MFLAGS) $(FFLAGS) $(SAFEOFLAGS)" \
		"OEXT=$(OEXT)" "LEXT=$(LEXT)" "EEXT=$(EEXT)" "X=$(X)" \
		"RM=$(RM)" libcacti.$(LEXT)


clean:
	-$(RM) *.o *.obj  $(PROGS)  
	cd libexo $(CS) $(MAKE) "RM=$(RM)" "CS=$(CS)" clean $(CS) cd ..
	cd libcacti $(CS) $(MAKE) "RM=$(RM)" "CS=$(CS)" clean $(CS) cd ..
	cd $(ESYNET_DIR) $(CS) $(MAKE) clean $(CS) $(MAKE) cleanlib $(CS) cd ..
	cd $(FSOI_DIR) $(CS) $(MAKE) clean $(CS) cd ..


# DO NOT DELETE THIS LINE -- make depend depends on it.

main.$(OEXT): host.h misc.h machine.h machine.def endian.h version.h dlite.h
main.$(OEXT): regs.h memory.h options.h stats.h eval.h loader.h sim.h
sim-outorder.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
sim-outorder.$(OEXT): options.h stats.h eval.h cache.h loader.h syscall.h
sim-outorder.$(OEXT): bpred.h resource.h bitmap.h ptrace.h range.h dlite.h
sim-outorder.$(OEXT): sim.h
sim-outorder.$(OEXT): smt.h
sim-outorder.$(OEXT): interconnect.h
sim-outorder.$(OEXT): network.h
sim-outorder.$(OEXT): mystats.h
memory.$(OEXT): host.h misc.h machine.h machine.def options.h stats.h eval.h
memory.$(OEXT): memory.h
regs.$(OEXT): host.h misc.h machine.h machine.def loader.h regs.h memory.h
regs.$(OEXT): options.h stats.h eval.h
cache.$(OEXT): host.h misc.h machine.h machine.def cache.h memory.h options.h
cache.$(OEXT): stats.h eval.h
bpred.$(OEXT): host.h misc.h machine.h machine.def bpred.h stats.h eval.h
network.$(OEXT): host.h misc.h machine.h machine.def network.h stats.h eval.h
mystats.$(OEXT): machine.h machine.def mystats.h
ptrace.$(OEXT): host.h misc.h machine.h machine.def range.h ptrace.h
eventq.$(OEXT): host.h misc.h machine.h machine.def eventq.h bitmap.h
resource.$(OEXT): host.h misc.h resource.h
endian.$(OEXT): endian.h loader.h host.h misc.h machine.h machine.def regs.h
endian.$(OEXT): memory.h options.h stats.h eval.h
dlite.$(OEXT): host.h misc.h machine.h machine.def version.h eval.h regs.h
dlite.$(OEXT): memory.h options.h stats.h sim.h symbol.h loader.h range.h
dlite.$(OEXT): dlite.h
symbol.$(OEXT): host.h misc.h ecoff.h loader.h machine.h
symbol.$(OEXT): machine.def regs.h memory.h options.h stats.h eval.h symbol.h
eval.$(OEXT): host.h misc.h eval.h machine.h machine.def
options.$(OEXT): host.h misc.h options.h
range.$(OEXT): host.h misc.h machine.h machine.def symbol.h loader.h regs.h
range.$(OEXT): memory.h options.h stats.h eval.h range.h
eio.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h options.h
eio.$(OEXT): stats.h eval.h loader.h libexo/libexo.h host.h misc.h machine.h
eio.$(OEXT): syscall.h sim.h endian.h eio.h
stats.$(OEXT): host.h misc.h machine.h machine.def eval.h stats.h
endian.$(OEXT): endian.h loader.h host.h misc.h machine.h machine.def regs.h
endian.$(OEXT): memory.h options.h stats.h eval.h
misc.$(OEXT): host.h misc.h machine.h machine.def
power.$(OEXT): host.h misc.h machine.h machine.def cache.h memory.h options.h
power.$(OEXT): stats.h eval.h power.h smt.h
loader.$(OEXT): host.h misc.h machine.h machine.def endian.h regs.h memory.h
loader.$(OEXT): options.h stats.h eval.h sim.h eio.h loader.h
loader.$(OEXT): ecoff.h
syscall.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
syscall.$(OEXT): options.h stats.h eval.h loader.h sim.h endian.h eio.h
syscall.$(OEXT): syscall.h
symbol.$(OEXT): host.h misc.h ecoff.h loader.h machine.h
symbol.$(OEXT): machine.def regs.h memory.h options.h stats.h eval.h symbol.h
loader.$(OEXT): host.h misc.h machine.h machine.def endian.h regs.h memory.h
loader.$(OEXT): options.h stats.h eval.h sim.h eio.h loader.h
loader.$(OEXT): ecoff.h elf64.h
syscall.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
syscall.$(OEXT): options.h stats.h eval.h loader.h sim.h endian.h eio.h
syscall.$(OEXT): syscall.h
symbol.$(OEXT): host.h misc.h loader.h machine.h machine.def regs.h memory.h
symbol.$(OEXT): options.h stats.h eval.h symbol.h ecoff.h
interconnect.$(OEXT): interconnect.h
cluster.$(OEXT): cluster.h
cluster.$(OEXT): headers.h
cluster-opts.$(OEXT): headers.h
cmt.$(OEXT): headers.h
flp.$(OEXT): headers.h
hotspot.$(OEXT): headers.h
interconnect.$(OEXT): headers.h
power.$(OEXT): headers.h
RCutil.$(OEXT): headers.h
sim-outorder.$(OEXT): headers.h
syscall.$(OEXT): headers.h
temperature.$(OEXT): headers.h
tokencoherence.$(OEXT): tokencoherence.h
sim-outorder.$(OEXT): tokencoherence.h
cache.$(OEXT): tokencoherence.h
esynet.$(OEXT): 
