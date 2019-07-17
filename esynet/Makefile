
include ../Makefile.include

MODULE := esynet

TARGET_NAME := esynet
TARGET := $(TARGET_NAME)$(EEXT)

SRCS := $(filter-out esynet_interface.cc, $(wildcard *.cc))

OBJS := $(addprefix $(BUILD_DIR)$(X)$(MODULE)$(X), $(notdir $(SRCS:%.cc=%.o)))
DEPS := $(addprefix $(BUILD_DIR)$(X)$(MODULE)$(X), $(notdir $(SRCS:%.cc=%.d)))

CFLAGS += -DPOWER_TEST -I.$(X)orion$(X)power -I.$(X)orion$(X)library

OFLAGS += -L.$(X)orion$(X)power -lpower

all:
	@echo Compile ESYNet Network-on-Chip Simulator
	$(MAKE) libs
	$(MAKE) netcfg_lib
	$(MAKE) tinyxml_lib
	$(MAKE) $(TARGET)

$(TARGET): $(OBJS)
	$(MKDIR) $(BIN_DIR)
	$(CCPP) $(OFLAGS) -o $(BIN_DIR)$(X)$(TARGET)

updatexml:
	$(EXEC)$(TARGET) -xml $(TARGET_NAME) -xmlexec $(EXEC)$(TARGET) -xmlcomp "$(MAKE)" -xmlname "ESYNet Simulator"

-include $(DEPS)

include ../Makefile.rule

