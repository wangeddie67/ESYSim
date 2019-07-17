
include ../../Makefile.include

MODULE := libs$(X)tinyxml

SRCS := $(filter-out xmltest.cpp, $(wildcard *.cpp))

OBJS := $(addprefix $(BUILD_DIR)$(X)$(MODULE)$(X), $(notdir $(SRCS:%.cpp=%.o)))
DEPS := $(addprefix $(BUILD_DIR)$(X)$(MODULE)$(X), $(notdir $(SRCS:%.cpp=%.d)))

all: $(OBJS)

-include $(DEPS)

include ../../Makefile.rule
