
include ../Makefile.include

export ESYNETLINKED += -DESYNETINTERFACE 

ESYMAP_NAME = esymap
ESYMAP_TARGET = $(ESYMAP_NAME)$(EEXT)

TGFF_NAME = tgffsocket
TGFF_TARGET = $(TGFF_NAME)$(EEXIT)

# network to buil : popnet relay network or free-space optical network
ESYNET_DIR = ..$(X)esynet
ESYNET_TARGET = $(ESYNET_DIR)$(X)esynet_lib
ESYNET_LIB = $(ESYNET_DIR)$(X)*.$(OEXT)

POWER_RELEASE = orion_power_beta_mar2003
PTARGET = power_model

CFLAGS += -DPOWER_TEST \
	-I$(ESYNET_DIR)$(X)$(POWER_RELEASE)$(X)power \
	-I$(ESYNET_DIR)$(X)$(POWER_RELEASE)$(X)library \
	-I$(INTERFACE_DIR) -I$(TINYXML_DIR) -I$(ESYNET_DIR) $(ALPHAFLAGS)

ESYMAP_SRCS = esymap_main.cc
SIM_SRCS = \
	esymap_config.cc\
	esymap_task_machine.cc\
	esymap_task.cc\
	esymap_mapping_cam.cc\
	esymap_mapping_basic.cc\
	esymap_mapping_cona.cc\
	esymap_mapping_bn.cc\
	esymap_mapping_casqa.cc\
	esymap_mapping_orders.cc\
	esymap_mapping_first_node.cc\
	esymap_mapping_flow_info.cc\
	esymap_statistic.cc \

TGFF_SRCS = tgffsocket_main.cc

ESYMAP_OBJS = esymap_main.$(OEXT)
SIM_OBJS = \
	esymap_config.$(OEXT)\
	esymap_task_machine.$(OEXT)\
	esymap_task.$(OEXT)\
	esymap_mapping_cam.$(OEXT)\
	esymap_mapping_basic.$(OEXT)\
	esymap_mapping_cona.$(OEXT)\
	esymap_mapping_bn.$(OEXT)\
	esymap_mapping_casqa.$(OEXT)\
	esymap_mapping_orders.$(OEXT)\
	esymap_mapping_first_node.$(OEXT)\
	esymap_mapping_flow_info.$(OEXT)\
	esymap_statistic.$(OEXT) \

TGFF_OBJ = tgffsocket_main.$(OEXT)

all: $(ESYMAP_TARGET) $(TGFF_TARGET)
	
$(ESYMAP_TARGET): $(ESYMAP_OBJS) $(SIM_OBJS) $(ESYNET_TARGET)
	$(CCPP) $(OUTOPT) $(ESYMAP_TARGET) $(SIM_OBJS) $(ESYMAP_OBJS) \
		$(ESYNET_LIB) \
		$(ESYNET_DIR)$(X)$(POWER_RELEASE)$(X)power$(X)libpower.$(LEXT) \
		$(INTERFACE_LIB) $(TINYXML_LIB)
	$(EXEC)$(ESYMAP_TARGET) -xml $(ESYMAP_NAME) \
		-xmlexec $(EXEC)$(ESYMAP_TARGET) -xmlcomp "$(MAKE)" \
		-xmlname "ESYMap Evaluator"

$(TGFF_TARGET): $(TGFF_OBJ) 
	$(CCPP) $(OUTOPT) $(TGFF_TARGET) $(TGFF_OBJ) \
		$(INTERFACE_LIB) $(TINYXML_LIB)
	$(EXEC)$(TGFF_TARGET) -xml $(TGFF_NAME) \
		-xmlexec $(EXEC)$(TGFF_NAME) -xmlcomp "$(MAKE)" \
		-xmlname "Task Graphics for Free"

$(ESYNET_TARGET):
	cd $(ESYNET_DIR); $(MAKE) interface

esymap_main.$(OEXT): esymap_main.cc
esymap_map_config.$(OEXT): esymap_config.cc esymap_config.h
esymap_task_machine.$(OEXT): esymap_task_machine.cc esymap_task_machine.h
esymap_task.$(OEXT): esymap_task.cc esymap_task.h
esymap_mapping_cam.$(OEXT): esymap_mapping_cam.cc esymap_task.h
esymap_mapping_basic.$(OEXT): esymap_mapping_basic.cc esymap_task.h
esymap_mapping_bn.$(OEXT): esymap_mapping_bn.cc esymap_task.h
esymap_mapping_cona.$(OEXT): esymap_mapping_cona.cc esymap_task.h
esymap_mapping_casqa.$(OEXT): esymap_mapping_casqa.cc esymap_task.h
esymap_mapping_orders.$(OEXT): esymap_mapping_orders.cc esymap_task.h	
esymap_mapping_first_node.$(OEXT): esymap_mapping_first_node.cc esymap_task.h
esymap_mapping_flow_info.$(OEXT): esymap_mapping_flow_info.cc esymap_task.h	
esymap_statistic.$(OEXT): esymap_statistic.cc esymap_statistic.h

tgffsocket_main.$(OEXT): tgffsocket_main.cc

clean: cleanlocal

cleanall: cleanlocal cleanesynet
	
cleanlocal:
	$(RM) $(ESYMAP_TARGET) *~ *.obj *.o 

cleanesynet:
	cd $(ESYNET_DIR) && make cleanall
	
rm:
	rm *.txt
