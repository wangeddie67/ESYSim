TARGET = mcpat
SHELL = /bin/sh
.PHONY: all depend clean
.SUFFIXES: .cc .o

ifndef NTHREADS
  NTHREADS = 8
endif


LIBS = -lm
INCS =

ifeq ($(TAG),dbg)
  DBG = -Wall
  SRCS = mainMc.cc
  OPT = -ggdb -g -O0 -DNTHREADS=$(NTHREADS) -Icacti
else
  DBG = 
  SRC = main.cc
  OPT = -O3 -msse2 -mfpmath=sse -DNTHREADS=$(NTHREADS) -Icacti
  #OPT = -O0 -DNTHREADS=$(NTHREADS)
endif

#CXXFLAGS = -Wall -Wno-unknown-pragmas -Winline $(DBG) $(OPT) 
CXXFLAGS = -Wno-unknown-pragmas $(DBG) $(OPT) 
CXX = g++
CC  = gcc

VPATH = cacti

SRCS  += \
  Ucache.cc \
  XML_Parse.cc \
  arbiter.cc \
  area.cc \
  array.cc \
  bank.cc \
  basic_circuit.cc \
  basic_components.cc \
  cacti_interface.cc \
  component.cc \
  core.cc \
  crossbar.cc \
  decoder.cc \
  htree2.cc \
  interconnect.cc \
  io.cc \
  iocontrollers.cc \
  logic.cc \
  mat.cc \
  memoryctrl.cc \
  noc.cc \
  nuca.cc \
  parameter.cc \
  processor.cc \
  router.cc \
  sharedcache.cc \
  subarray.cc \
  technology.cc \
  uca.cc \
  wire.cc \
  xmlParser.cc 

OBJS = $(patsubst %.cc,obj_$(TAG)/%.o,$(SRCS))

all: obj_$(TAG)/$(TARGET)

obj_opt/$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) -o $@ $(INCS) $(CXXFLAGS) $(LIBS) -pthread
	cp -f obj_opt/$(TARGET) $(TARGET)
	
obj_dbg/$(TARGET) : $(OBJS)

obj_$(TAG)/%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	-rm -f *.o $(TARGET)


