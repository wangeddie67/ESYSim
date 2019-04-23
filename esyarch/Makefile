CC = gcc
CPP = g++
INC = 
OFLAGS = -O0 -g3 -w
#MFLAGS = `./sysprobe -flags`
#MLIBS  = `./sysprobe -libs` -lm
#ENDIAN = `./sysprobe -s`
MAKE = make
AR = ar qcv
AROPT =
RANLIB = ranlib
RM = rm -f
RMDIR = rm -f
LN = ln -s
LNDIR = ln -s
DIFF = diff
OEXT = o
LEXT = a
EEXT =
CS = ;
X=/
export ESYNETLINKED += -DESYNETINTERFACE

INCLUDE_PATHS = -I./interface -I../tinyxml -I../interface -I../esynet/orion_power_beta_mar2003/library \
	-I../esynet/orion_power_beta_mar2003/power -DPOWER_TEST

FFLAGS = -DDEBUG
#
# complete flags
#
CFLAGS = $(FFLAGS) $(OFLAGS)

#
# all the sources
#
SRCS =	main.cc fpgac.cc pisa.cc alpha.cc esynetRecord.cc \
	core.cc node.cc Noc.cc Simulation.cc resource.cc Statistics.cc Powermc.cpp MPITablc.cc endian.cc misc.cc 
INTFACE_SRCS = arch2esynetinterface.cc

HDRS =	fpgac.h mips.h alpha.h node.h esynetRecord.h \
	mipsdef.h mipsudef.h alphadef.h alphaundef.h Noc.h Simulation.h memundef.h resource.h MPITable.h \
	Statistics.h node_cfg.h power_mc.h define.h \
	ecoff.h endian.h eventq.h bitmap.h resource.h misc.h host.h
INTFACE_HDRS = arch2esynetinterface.h
#
# common objects
#
OBJS =	 main.$(OEXT) fpgac.$(OEXT) pisa.$(OEXT) alpha.$(OEXT) \
	esynetRecord.$(OEXT) \
	core.$(OEXT) node.$(OEXT) Noc.$(OEXT) Simulation.$(OEXT) resource.$(OEXT) Statistics.$(OEXT) \
	Powermc.$(OEXT) MPITable.$(OEXT) endian.$(OEXT) misc.$(OEXT) 

OTHERS = arch2esynetinterface.o ../esynet/*.o ../interface/*.o ../tinyxml/*.o ../esynet/orion_power_beta_mar2003/power/libpower.a
INTFACE_OBJS = arch2esynetinterface.o
# programs to build
PROGS = sim-outorder$(EEXT)

# all targets, NOTE: library ordering is important...
all: $(PROGS)
	@echo "my work is done here..."

sim-outorder$(EEXT):	$(OBJS) $(OTHERS) libexo/libexo.$(LEXT)  mcpat/obj_opt/*.$(OEXT)
	$(CPP) -o sim-outorder$(EEXT) $(CFLAGS) $(INCLUDE_PATHS) $(OBJS) $(OTHERS) libexo/libexo.$(LEXT)  mcpat/obj_dbg/*.$(OEXT) -pthread
	./sim-outorder$(EEXT) -xml esyarch -xmlexec ./sim-outorder -xmlcomp \
		make -xmlname "ESYArch Manycore SoC Simulator"

#sysprobe$(EEXT):	sysprobe.cc
#	$(CC) $(FFLAGS) -o sysprobe$(EEXT) sysprobe.cc
#	@echo endian probe results: $(ENDIAN)
#	@echo probe flags: $(MFLAGS)
#	@echo probe libs: $(MLIBS)

popnet popnet/*.o :
	cd popnet $(CS) \
	$(MAKE)

mcpat mcpat/obj_opt/*.o :
	cd mcpat $(CS) \
	$(MAKE) 	

esynet ../esynet/*.o ../interface/*.o ../tinyxml/*.o ../esynet/orion_power_beta_mar2003/power/libpower.a:
	cd ../esynet $(CS) \
	$(MAKE) interface

#hotspot hotspot/*.o :
#	cd hotspot $(CS) \
#	$(MAKE)

exo libexo/libexo.$(LEXT): 
	cd libexo $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "AR=$(AR)" "AROPT=$(AROPT)" "RANLIB=$(RANLIB)" "CFLAGS=$(MFLAGS) $(FFLAGS) $(OFLAGS)" "OEXT=$(OEXT)" "LEXT=$(LEXT)" "EEXT=$(EEXT)" "X=$(X)" "RM=$(RM)" libexo.$(LEXT)


.cc.$(OEXT):
	$(CPP) $(CFLAGS) $(INCLUDE_PATHS) -c $*.cc
#	$(CPP) $(CFLAGS) -E $*.cc -o $*.i
#	$(CPP) $(CFLAGS) -S $*.i -o $*.s
#	$(CPP) $(CFLAGS) -c $*.s -o $*.o
.cpp.$(OEXT):
	$(CPP) $(CFLAGS) $(INCLUDE_PATHS) -c $*.cpp
#	$(CPP) -E $*.cc > $*.i
#	$(CPP) -S $*.i > $*.s
#	$(CPP) -c $*.s > $*.o

	
filelist:
	@echo $(SRCS) $(HDRS) Makefile

diffs:
	-rcsdiff RCS/*
	-cd config; rcsdiff RCS/*
	-cd libexo; rcsdiff RCS/*
	-cd target-alpha; rcsdiff RCS/*
	-cd target-pisa; rcsdiff RCS/*


clean:
	-$(RM) *.o *.obj *.exe *.i *.s core *~ MAKE.log Makefile.bak $(PROGS)

cleanesynet:
	cd ../esynet $(CS) $(MAKE) cleanall $(CS) cd ../../
	
cleanall:
	-$(RM) *.o *.obj *.exe *.i *.s core *~ MAKE.log Makefile.bak $(PROGS)
	cd libexo $(CS) $(MAKE) "RM=$(RM)" "CS=$(CS)" clean $(CS) cd ..
	cd ../esynet $(CS) $(MAKE) cleanall $(CS) cd ../../
#	cd mcpat $(CS) $(MAKE) "RM=$(RM)" "CS=$(CS)" clean $(CS) cd ..
#	cd hotspot $(CS) $(MAKE) "RM=$(RM)" "CS=$(CS)" clean $(CS) cd ..
	
unpure:
	rm -f sim.pure *pure*.o sim.pure.pure_hardlink sim.pure.pure_linkinfo

depend:	mkdepend
	makedepend/makedepend -n $(BINUTILS_INC) $(SRCS)

mkdepend:
	cd makedepend $(CS) $(MAKE) "RM=$(RM)" "CS=$(CS)" $(CS) cd ..

mpitest:
	cd testmpi && make clean && make
# DO NOT DELETE THIS LINE -- make depend depends on it.
*.cc:define.h
pisa.cc:host.h misc.h
core.cc:host.h misc.h
