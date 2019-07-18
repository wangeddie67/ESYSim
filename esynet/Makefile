
include ../Makefile.include

MODULE := esynet

SUBMODULE := orion

EXCLUDE_SRCFILE := esynet_interface.cc

CFLAGS += -DPOWER_TEST -I.$(X)orion$(X)power -I.$(X)orion$(X)library

OFLAGS += -L.$(X)orion$(X)power -lpower

updatexml:
	$(EXEC)$(TARGET) -xml $(TARGET_NAME) -xmlexec $(EXEC)$(TARGET) -xmlcomp "$(MAKE)" -xmlname "ESYNet Simulator"

include ../Makefile.rule

