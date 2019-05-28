#Makefile for ESYNet

include ../Makefile.include

ESYNET_NAME = esynet
ESYNET_TARGET = $(ESYNET_NAME)$(EEXT)

POWER_RELEASE = orion_power_beta_mar2003

CFLAGS += -DPOWER_TEST \
	-I.$(X)$(POWER_RELEASE)$(X)power -I.$(X)$(POWER_RELEASE)$(X)library \
	-I$(INTERFACE_DIR) -I$(TINYXML_DIR) $(ESYNETLINKED)

LINKFLAGS = -L.$(X)$(POWER_RELEASE)$(X)power -lm -lpower

PTARGET = power_model

ESYNET_SRCS = esynet_main.cc
INTER_SRC = esynet_interface.cc
SIM_SRCS = \
	esynet_config.cc \
	esynet_foundation.cc \
	esynet_event_queue.cc \
	esynet_ni_unit.cc \
	esynet_packet_generator.cc \
	esynet_random_unit.cc \
	esynet_arbiter.cc \
	esynet_router_unit.cc \
	esynet_routing_xy.cc \
	esynet_routing_table.cc

ESYNET_OBJS = esynet_main.$(OEXT)
INTER_OBJS = esynet_interface.$(OEXT)
SIM_OBJS = \
	esynet_config.$(OEXT) \
	esynet_foundation.$(OEXT) \
	esynet_event_queue.$(OEXT) \
	esynet_ni_unit.$(OEXT) \
	esynet_packet_generator.$(OEXT) \
	esynet_random_unit.$(OEXT) \
	esynet_arbiter.$(OEXT) \
	esynet_router_unit.$(OEXT) \
	esynet_routing_xy.$(OEXT) \
	esynet_routing_table.$(OEXT)

all: $(ESYNET_TARGET)
	
$(ESYNET_TARGET): $(SIM_OBJS) $(ESYNET_OBJS) $(PTARGET) $(INTERFACE_TARGET) \
$(TINYXML_TARGET)
	$(CCPP) $(OUTOPT)$(ESYNET_TARGET) $(SIM_OBJS) $(ESYNET_OBJS) \
		.$(X)$(POWER_RELEASE)$(X)power$(X)libpower.$(LEXT) \
		$(INTERFACE_LIB) $(TINYXML_LIB)
	$(MKDIR) ../bin
	$(MV) $(ESYNET_TARGET) ../bin
	$(RM) $(ESYNET_OBJS)

$(PTARGET):
	cd .$(X)$(POWER_RELEASE)$(X)power && $(MAKE)

$(INTERFACE_TARGET):
	cd $(INTERFACE_DIR) && $(MAKE)

$(TINYXML_TARGET):
	cd $(TINYXML_DIR) && $(MAKE)
	
interface: $(SIM_OBJS) $(INTER_OBJS) $(PTARGET) $(INTERFACE_TARGET) \
$(TINYXML_TARGET)

updatexml:
	$(EXEC)$(ESYNET_TARGET) -xml $(ESYNET_NAME) \
		-xmlexec $(EXEC)$(ESYNET_TARGET) -xmlcomp "$(MAKE)" \
		-xmlname "ESYNet Simulator"

include $(SIM_SRCS:.cc=.d)
include $(ESYNET_SRCS:.cc=.d)

clean: cleanlocal

cleanall: cleanlocal cleanlib cleanpower
	
cleanlocal:
	$(RM) ../bin
	$(RM) *~ *.obj *.o *.d *.exe *.dll

cleanlib:
	cd $(INTERFACE_DIR) && $(MAKE) clean
	cd $(TINYXML_DIR) && $(MAKE) clean

cleanpower:
	cd .$(X)$(POWER_RELEASE)$(X)power && $(MAKE) clean

