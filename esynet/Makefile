
include ../Makefile.include

MODULE := esynet

SUBMODULE := orion

EXCLUDE_SRCFILE := esynet_interface.cc

CFLAGS += -DPOWER_TEST -I.$(X)orion

updatexml:
	$(EXEC)$(TARGET) -xml $(TARGET_NAME) -xmlexec $(EXEC)$(TARGET) -xmlcomp "$(MAKE)" -xmlname "ESYNet Simulator"

include ../Makefile.rule

