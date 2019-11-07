
include ../../Makefile.include

# path to store .obj, .cxx, and .so
MODULE := trafficgen

# protoc module
PROTOC_MODULE := esy_traffic

# swig module
SWIG_MODULE := esy_packetgen

# python files to copy to module path
PYTHON_SRC := py_esy_trafficgenerator.py \
              esy_traffic_pb2.py \
              __init__.py

LINK_DIR :=
LINK_OBJ += $(BUILD_DIR)$(X)randgen$(X)esy_random.$(OEXT)
LINK_LIB += -lprotobuf

include ../../Makefile.rule
