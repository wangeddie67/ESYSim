
include ../Makefile.include

MODULE := tools
SUBMODULE := netcfgeditor

SRCS := $(wildcard *.cc)

OBJS := $(addprefix $(BUILD_DIR)$(X)$(MODULE)$(X), $(notdir $(SRCS:%.cc=%.o)))
DEPS := $(addprefix $(BUILD_DIR)$(X)$(MODULE)$(X), $(notdir $(SRCS:%.cc=%.d)))

all: $(OBJS)

-include $(DEPS)

include ../Makefile.rule
