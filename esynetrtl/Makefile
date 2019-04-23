#Makefile for Event trace to traffic window

include ../Makefile.include

TARGETNAME = nocgenerator

TARGET = $(TARGETNAME)$(EEXT)

CFLAGS += -std=c++11

SRCS = \
	nocgen/main.cpp \
	nocgen/rtl_gen.cpp \
	nocgen/file_copy.cpp \

OBJS = \
	nocgen/main.$(OEXT) \
	nocgen/rtl_gen.$(OEXT) \
	nocgen/file_copy.$(OEXT) \

all: $(TARGET)

$(TARGET):$(OBJS) $(INTERFACE_TARGET) $(TINYXML_TARGET)
	$(MV) *.o nocgen/
	$(CCPP) $(OUTOPT)$(TARGET) $(OBJS) $(INTERFACE_LIB) $(TINYXML_LIB)
	$(EXEC)$(TARGET) -xml $(TARGETNAME) -xmlexec $(EXEC)$(TARGET) -xmlcomp "$(MAKE)" -xmlname "RTL NoC Generator"

$(INTERFACE_TARGET):
	cd $(INTERFACE_DIR) && $(MAKE)

$(TINYXML_TARGET):
	cd $(TINYXML_DIR) && $(MAKE)	

nocgen/main.$(OEXT):nocgen/main.cpp nocgen/rtl_gen.h
nocgen/rtl_gen.$(OEXT):nocgen/rtl_gen.cpp nocgen/rtl_gen.h
nocgen/file_copy.$(OEXT):nocgen/file_copy.cpp nocgen/rtl_gen.h

clean: cleanlocal cleanlib

cleanlocal:
	$(RM) $(TARGET) *.obj *.o *~ *.exe *.dll

cleanlib:
	cd ..$(X)interface && $(MAKE) clean

