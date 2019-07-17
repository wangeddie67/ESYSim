
include ../../Makefile.include

MODULE := tools$(X)netcfgeditor
TARGET := netcfgeditor$(EEXT)

SRCS := $(wildcard *.cc)

OBJS := $(addprefix $(BUILD_DIR)$(X)$(MODULE)$(X), $(notdir $(SRCS:%.cc=%.o)))
DEPS := $(addprefix $(BUILD_DIR)$(X)$(MODULE)$(X), $(notdir $(SRCS:%.cc=%.d)))

all:
	@echo Compile Network Configuration Editor
	$(MAKE) libs
	$(MAKE) netcfg_lib
	$(MAKE) tinyxml_lib
	$(MAKE) $(TARGET)
	
$(TARGET): $(OBJS)
	$(MKDIR) $(BIN_DIR)
	$(CCPP) $(OFLAGS) -o $(BIN_DIR)$(X)$(TARGET)

-include $(DEPS)

include ../../Makefile.rule
