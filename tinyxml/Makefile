#Makefile for TinyXML

include ../Makefile.include

SRCS = \
	tinyxml.cpp \
	tinyxmlparser.cpp \
	tinyxmlerror.cpp \
	tinystr.cpp

OBJS = \
	tinyxml.$(OEXT) \
	tinyxmlparser.$(OEXT) \
	tinyxmlerror.$(OEXT) \
	tinystr.$(OEXT)


all: $(OBJS)

dist:
	bash makedistlinux

clean:
	$(RM) *.obj *.o *~

depend:
#	#makedepend ${INCS} ${SRCS}

tinyxml.$(OEXT): tinyxml.h tinystr.h
tinyxmlparser.$(OEXT): tinyxml.h tinystr.h
xmltest.$(OEXT): tinyxml.h tinystr.h
tinyxmlerror.$(OEXT): tinyxml.h tinystr.h
