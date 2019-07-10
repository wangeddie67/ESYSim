
include ../../Makefile.include

TARGET := netcfgeditor

SRCS := $(wildcard *.cc)

OBJS := $(addprefix $(OBJ_DIR)$(X), $(notdir $(SRCS:%.cc=%.o)))
DEPS := $(addprefix $(DEP_DIR)$(X), $(notdir $(SRCS:%.cc=%.d)))

all:
	@echo Compile Network Configuration Editor
	make libs
	make netcfg_lib
	make tinyxml_lib
	make $(TARGET)

$(TARGET): $(OBJS)
	$(MKDIR) $(BIN_DIR)
	$(CCPP) $(OFLAGS) -o $(BIN_DIR)$(X)$(TARGET)

include $(DEPS)

include ../../Makefile.rule
