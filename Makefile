
include Makefile.include

SUBMODULE := esynet libs tools

all:
	@echo Make Dependence Files
	$(MAKE) depend
	@echo Compile Object Files
	$(MAKE) objs

include Makefile.rule
