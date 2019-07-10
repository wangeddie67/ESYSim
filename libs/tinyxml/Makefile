
include ../../Makefile.include

SRCS := $(filter-out xmltest.cpp, $(wildcard *.cpp))

OBJS := $(addprefix $(OBJ_DIR)$(X), $(notdir $(SRCS:%.cpp=%.o)))
DEPS := $(addprefix $(DEP_DIR)$(X), $(notdir $(SRCS:%.cpp=%.d)))

all: $(OBJS)

include $(DEPS)

include ../../Makefile.rule
