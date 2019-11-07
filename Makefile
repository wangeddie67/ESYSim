
include Makefile.include

all:
	@$(MAKE) -C $(ESYSIM_ROOT)$(X)srcs depend
	@$(MAKE) -C $(ESYSIM_ROOT)$(X)srcs objs

#.esynet: $(BIN_DIR)$(X)esynet $(BIN_DIR)$(X)netcfgeditor

#$(BIN_DIR)$(X)esynet: $(LINK_OBJ) $(BUILD_DIR)$(X)esynet$(X)*.$(OEXT) $(BUILD_DIR)$(X)esynet$(X)orion$(X)*.$(OEXT)
#	@echo "[LINK ] " $@
#	$(CCPP) $(OFLAGS) -o $@ \
#		$(BUILD_DIR)$(X)esynet$(X)*.$(OEXT) \
#		$(BUILD_DIR)$(X)esynet$(X)orion$(X)*.$(OEXT)

#$(BIN_DIR)$(X)netcfgeditor: $(LINK_OBJ) $(BUILD_DIR)$(X)tools$(X)netcfgeditor$(X)*.$(OEXT)
#	@echo "[LINK ] " $@
#	$(CCPP) $(OFLAGS) -o $@ \
#		$(BUILD_DIR)$(X)tools$(X)netcfgeditor$(X)*.$(OEXT)

include Makefile.rule
