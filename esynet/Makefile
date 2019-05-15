#Makefile for ESYNet

include ../Makefile.include

ESYNET_NAME = esynet
ESYNET_TARGET = $(ESYNET_NAME)$(EEXT)
PATH_NAME = esynetpath
PATH_TARGET = $(PATH_NAME)$(EEXT)

POWER_RELEASE = orion_power_beta_mar2003

CFLAGS += -DPOWER_TEST \
	-I.$(X)$(POWER_RELEASE)$(X)power -I.$(X)$(POWER_RELEASE)$(X)library \
	-I$(INTERFACE_DIR) -I$(TINYXML_DIR) $(ESYNETLINKED)

LINKFLAGS = -L.$(X)$(POWER_RELEASE)$(X)power -lm -lpower

PTARGET = power_model

ESYNET_SRCS = esynet_main.cc
PATH_SRCS = esynet_path.cc
INTER_SRC = esynet_interface.cc
SIM_SRCS = \
	esynet_config.cc \
	esynet_foundation.cc \
	esynet_mess_event.cc \
	esynet_mess_queue.cc \
	esynet_ni_unit.cc \
	esynet_packet_generator.cc \
	esynet_port_unit.cc \
	esynet_random_unit.cc \
	esynet_arbiter.cc \
	esynet_router_fault.cc \
	esynet_router_power.cc \
	esynet_router_unit.cc \
	esynet_router_bist.cc \
	esynet_router_bypass.cc \
	esynet_routing_rescuer.cc \
	esynet_routing_nonblock.cc \
	esynet_routing_ftlr.cc \
	esynet_routing_hipfar.cc \
	esynet_routing_xy.cc \
	esynet_statistics.cc 

ESYNET_OBJS = esynet_main.$(OEXT)
PATH_OBJS = esynet_path.$(OEXT)
INTER_OBJS = esynet_interface.$(OEXT)
SIM_OBJS = \
	esynet_config.$(OEXT) \
	esynet_foundation.$(OEXT) \
	esynet_mess_event.$(OEXT) \
	esynet_mess_queue.$(OEXT) \
	esynet_ni_unit.$(OEXT) \
	esynet_packet_generator.$(OEXT) \
	esynet_port_unit.$(OEXT) \
	esynet_random_unit.$(OEXT) \
	esynet_arbiter.$(OEXT) \
	esynet_router_fault.$(OEXT) \
	esynet_router_power.$(OEXT) \
	esynet_router_unit.$(OEXT) \
	esynet_router_bist.$(OEXT) \
	esynet_router_bypass.$(OEXT) \
	esynet_routing_rescuer.$(OEXT) \
	esynet_routing_nonblock.$(OEXT) \
	esynet_routing_ftlr.$(OEXT) \
	esynet_routing_hipfar.$(OEXT) \
	esynet_routing_xy.$(OEXT) \
	esynet_statistics.$(OEXT) 


all: $(ESYNET_TARGET) $(PATH_TARGET)
	
$(ESYNET_TARGET): $(SIM_OBJS) $(ESYNET_OBJS) $(PTARGET) $(INTERFACE_TARGET) \
$(TINYXML_TARGET)
	$(CCPP) $(OUTOPT)$(ESYNET_TARGET) $(SIM_OBJS) $(ESYNET_OBJS) \
		.$(X)$(POWER_RELEASE)$(X)power$(X)libpower.$(LEXT) \
		$(INTERFACE_LIB) $(TINYXML_LIB)
	$(RM) $(ESYNET_OBJS)

$(PATH_TARGET): $(SIM_OBJS) $(PATH_OBJS) $(PTARGET) $(INTERFACE_TARGET) \
$(TINYXML_TARGET)
	$(CCPP) $(OUTOPT)$(PATH_TARGET) $(SIM_OBJS) $(PATH_OBJS) \
		.$(X)$(POWER_RELEASE)$(X)power$(X)libpower.$(LEXT) \
		$(INTERFACE_LIB) $(TINYXML_LIB)
	$(RM) $(PATH_OBJS)

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
	$(EXEC)$(PATH_TARGET) -xml $(PATH_NAME) \
		-xmlexec $(EXEC)$(PATH_TARGET) -xmlcomp "$(MAKE)" \
		-xmlname "ESYNet Path Simulator"

esynet_config.$(OEXT): esynet_config.cc esynet_config.h 
esynet_ecc_unit.$(OEXT): esynet_ecc_unit.cc esynet_ecc_unit.h 
esynet_fault_unit.$(OEXT): esynet_fault_unit.cc esynet_fault_unit.h 
esynet_flit.$(OEXT): esynet_flit.cc esynet_flit.h 
esynet_foundation.$(OEXT): esynet_foundation.cc esynet_foundation.h 
esynet_main.$(OEXT): esynet_main.cc 
esynet_mess_event.$(OEXT): esynet_mess_event.cc esynet_mess_event.h 
esynet_mess_queue.$(OEXT): esynet_mess_queue.cc esynet_mess_queue.h 
esynet_pe_unit.$(OEXT): esynet_pe_unit.cc esynet_pe_unit.h 
esynet_port_unit.$(OEXT): esynet_port_unit.cc esynet_port_unit.h 
esynet_path.$(OEXT): esynet_path.cc
esynet_packet_generator.$(OEXT): esynet_packet_generator.cc esynet_packet_generator.h 
esynet_random_unit.$(OEXT): esynet_random_unit.cc esynet_random_unit.h
esynet_arbiter.$(OEXT): esynet_arbiter.cc esynet_arbiter.h
esynet_router_fault.$(OEXT): esynet_router_fault.cc esynet_router_unit.h
esynet_router_power.$(OEXT): esynet_router_power.cc esynet_router_power.h 
esynet_router_unit.$(OEXT): esynet_router_unit.cc esynet_router_unit.h 
esynet_router_bist.$(OEXT): esynet_router_bist.cc esynet_router_unit.h
esynet_router_bypass.$(OEXT): esynet_router_bypass.cc esynet_router_unit.h
esynet_routing_ftlr.$(OEXT): esynet_routing_ftlr.cc esynet_router_unit.h
esynet_routing_hipfar.$(OEXT): esynet_routing_hipfar.cc esynet_router_unit.h
esynet_routing_xy.$(OEXT): esynet_routing_xy.cc esynet_router_unit.h
esynet_routing_rescuer.$(OEXT): esynet_routing_rescuer.cc esynet_router_unit.h
esynet_routing_nonblock.$(OEXT): esynet_routing_nonblock.cc esynet_router_unit.h
esynet_statistics.$(OEXT): esynet_statistics.cc esynet_statistics.h

clean: cleanlocal

cleanall: cleanlocal cleanlib cleanpower
	
cleanlocal:
	$(RM) $(ESYNET_TARGET) $(PATH_TARGET) *~ *.obj *.o *.exe *.dll

cleanlib:
	cd $(INTERFACE_DIR) && $(MAKE) clean
	cd $(TINYXML_DIR) && $(MAKE) clean

cleanpower:
	cd .$(X)$(POWER_RELEASE)$(X)power && $(MAKE) clean

