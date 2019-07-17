
include Makefile.include

SUBMODULE := esynet libs tools

all:
	@echo Make Dependence Files
	$(MAKE) depend

	@echo Compile Object Files
	$(MAKE) objs

	@echo Link Executable Files
	$(MKDIR) $(BIN_DIR)
	$(CCPP) $(OFLAGS) $(BUILD_DIR)$(X)esynet$(X)*.$(OEXT) \
		-o $(BIN_DIR)$(X)esynet
	$(CCPP) $(OFLAGS) $(BUILD_DIR)$(X)tools$(X)netcfgeditor$(X)*.$(OEXT) \
		-o $(BIN_DIR)$(X)netcfgeditor

include Makefile.rule
