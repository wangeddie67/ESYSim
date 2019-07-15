#Makefile for Event trace to traffic window

include ../Makefile.include

TARGETNAME = traceanalyse

TARGET = $(TARGETNAME)$(EEXT)

SRCS = \
	main.cc \
	path.cc \
	reduce.cc \
	window.cc \

OBJS = \
	main.$(OEXT) \
	path.$(OEXT) \
	reduce.$(OEXT) \
	window.$(OEXT) \

all: $(TARGET)

$(TARGET):$(OBJS) $(INTERFACE_TARGET) $(TINYXML_TARGET)
	$(CCPP) $(OUTOPT)$(TARGET) $(OBJS) $(INTERFACE_LIB) $(TINYXML_LIB)
	$(EXEC)$(TARGET) -xml $(TARGETNAME) -xmlexec $(EXEC)$(TARGET) -xmlcomp "$(MAKE)" -xmlname "Event trace analyse"

$(INTERFACE_TARGET):
	cd $(INTERFACE_DIR) && $(MAKE)

$(TINYXML_TARGET):
	cd $(TINYXML_DIR) && $(MAKE)	

main.$(OEXT):main.cc
path.$(OEXT):path.cc path.h
reduce.$(OEXT):reduce.cc reduce.h
window.$(OEXT):window.cc window.h

clean: cleanlocal cleanlib

cleanlocal:
	$(RM) $(TARGET) *.obj *.o *~ *.exe *.dll

cleanlib:
	cd ..$(X)interface && $(MAKE) clean

