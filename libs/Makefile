
include ../Makefile.include

MODULE := libs
SUBMODULE := netcfg tinyxml

SRCS := $(wildcard *.cc)

OBJS := $(addprefix $(BUILD_DIR)$(X)$(MODULE)$(X), $(notdir $(SRCS:%.cc=%.o)))
DEPS := $(addprefix $(BUILD_DIR)$(X)$(MODULE)$(X), $(notdir $(SRCS:%.cc=%.d)))

all: $(OBJS)

-include $(DEPS)

include ../Makefile.rule
