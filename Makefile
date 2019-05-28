# Makefile for ESYSim

all:
	@echo ""
	@echo "Please specified the build target"
	@echo "    .esynet	Network simulator"
	@echo ""

.esynet: 
	@echo ""
	@echo "Build ESYNet network simulator"
	@echo ""
	cd esynet; make;

clean:
	@echo ""
	@echo "Clean ESYSim"
	@echo ""
	cd esynet; make clean;
