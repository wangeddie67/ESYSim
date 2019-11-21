
include ../../Makefile.include

# path to store .obj, .cxx, and .so
MODULE := esynet

EXCLUDE_SRCFILE := esynet_config.cc \
                   esynet_interface.cc \
                   esynet_main1.cc \
                   esynet_random_unit.cc \
                   esy_foundation.cc 

# swig module
SWIG_MODULE := esynet

# python files to copy to module path
PYTHON_SRC := esynet_main.py

LINK_DIR := 
LINK_OBJ += \
	$(BUILD_DIR)$(X)engine$(X)_esy_engine.so \
	$(BUILD_DIR)$(X)randgen$(X)_esy_random.so \
	$(BUILD_DIR)$(X)syscfg$(X)_esy_syscfg.so \
	$(BUILD_DIR)$(X)trafficgen$(X)_esy_packetgen.so
LINK_LIB += -lprotobuf \

include ../../Makefile.rule
