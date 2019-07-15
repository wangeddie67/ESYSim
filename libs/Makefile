
include ../Makefile.include

SRCS := $(wildcard *.cc)

OBJS := $(addprefix $(OBJ_DIR)$(X), $(notdir $(SRCS:%.cc=%.o)))
DEPS := $(addprefix $(DEP_DIR)$(X), $(notdir $(SRCS:%.cc=%.d)))

all: $(OBJS)

-include $(DEPS)

include ../Makefile.rule
