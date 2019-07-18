
include Makefile.include

SUBMODULE := esynet libs tools

all:
	@echo Make Dependence Files
	$(MAKE) depend

	@echo Compile Object Files
	$(MAKE) objs

	@echo Link Executable Files
	$(MKDIR) $(BIN_DIR)
	$(CCPP) $(OFLAGS) -o $(BIN_DIR)$(X)esynet \
		$(BUILD_DIR)$(X)esynet$(X)*.$(OEXT) \
		$(BUILD_DIR)$(X)esynet$(X)orion$(X)*.$(OEXT) 

	$(CCPP) $(OFLAGS) -o $(BIN_DIR)$(X)netcfgeditor \
		$(BUILD_DIR)$(X)tools$(X)netcfgeditor$(X)*.$(OEXT)

include Makefile.rule
