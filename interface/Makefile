include ../Makefile.include

SRCS = \
	esy_argument.cc \
	esy_conv.cc \
	esy_faultcfg.cc \
	esy_interdata.cc \
	esy_iodatafile_item.cc \
	esy_iodatafile.cc \
	esy_linktool.cc \
	esy_networkcfg.cc \
	esy_xmlerror.cc \
	esy_soccfg.cc 

OBJS = \
	esy_argument.$(OEXT) \
	esy_conv.$(OEXT) \
	esy_faultcfg.$(OEXT) \
	esy_interdata.$(OEXT) \
	esy_iodatafile_item.$(OEXT) \
	esy_iodatafile.$(OEXT) \
	esy_linktool.$(OEXT) \
	esy_networkcfg.$(OEXT) \
	esy_xmlerror.$(OEXT) \
	esy_soccfg.$(OEXT) 
	
all: $(OBJS) $(TINYXML_TARGET)

$(TINYXML_TARGET):
	cd $(TINYXML_DIR) && $(MAKE)

esy_argument.$(OEXT):esy_argument.cc esy_argument.h esy_conv.h
esy_conv.$(OEXT):esy_conv.cc esy_conv.h
esy_faultcfg.$(OEXT):esy_faultcfg.cc esy_faultcfg.h esy_conv.h esy_iodatafile.h
esy_interdata.$(OEXT):esy_interdata.cc esy_interdata.h esy_iodatafile.h
esy_iodatafile_item.$(OEXT):esy_iodatafile_item.cc esy_iodatafile_item.h
esy_iodatafile.$(OEXT):esy_iodatafile.cc esy_iodatafile.h esy_iodatafile_item.h
esy_linktool.$(OEXT):esy_linktool.cc esy_linktool.h
esy_networkcfg.$(OEXT):esy_networkcfg.cc esy_networkcfg.h esy_conv.h \
	esy_xmlerror.h
esy_xmlerror.$(OEXT):esy_xmlerror.cc esy_xmlerror.h
esy_soccfg.$(OEXT):esy_soccfg.cc esy_soccfg.h esy_conv.h esy_xmlerror.h

clean: cleanlocal

cleanlocal:
	$(RM) *.obj *.o *~

