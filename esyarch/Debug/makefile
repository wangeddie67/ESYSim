################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include visualnoc/trace2path/subdir.mk
-include visualnoc/tinyxml/subdir.mk
-include visualnoc/qtparallel/obj/subdir.mk
-include visualnoc/qtparallel/moc/subdir.mk
-include visualnoc/qtparallel/subdir.mk
-include visualnoc/interface/subdir.mk
-include visualnoc/integrator/threads/subdir.mk
-include visualnoc/integrator/subwidgets/subdir.mk
-include visualnoc/integrator/subdialogs/subdir.mk
-include visualnoc/integrator/qwt/textengines/mathml/subdir.mk
-include visualnoc/integrator/qwt/src/obj/subdir.mk
-include visualnoc/integrator/qwt/src/moc/subdir.mk
-include visualnoc/integrator/qwt/src/subdir.mk
-include visualnoc/integrator/qwt/playground/timescale/subdir.mk
-include visualnoc/integrator/qwt/playground/symbols/subdir.mk
-include visualnoc/integrator/qwt/playground/svgmap/subdir.mk
-include visualnoc/integrator/qwt/playground/shapes/subdir.mk
-include visualnoc/integrator/qwt/playground/scaleengine/subdir.mk
-include visualnoc/integrator/qwt/playground/rescaler/subdir.mk
-include visualnoc/integrator/qwt/playground/plotmatrix/subdir.mk
-include visualnoc/integrator/qwt/playground/graphicscale/subdir.mk
-include visualnoc/integrator/qwt/playground/curvetracker/subdir.mk
-include visualnoc/integrator/qwt/examples/tvplot/subdir.mk
-include visualnoc/integrator/qwt/examples/sysinfo/subdir.mk
-include visualnoc/integrator/qwt/examples/stockchart/subdir.mk
-include visualnoc/integrator/qwt/examples/spectrogram/subdir.mk
-include visualnoc/integrator/qwt/examples/sinusplot/subdir.mk
-include visualnoc/integrator/qwt/examples/simpleplot/subdir.mk
-include visualnoc/integrator/qwt/examples/scatterplot/subdir.mk
-include visualnoc/integrator/qwt/examples/refreshtest/subdir.mk
-include visualnoc/integrator/qwt/examples/realtime/subdir.mk
-include visualnoc/integrator/qwt/examples/rasterview/subdir.mk
-include visualnoc/integrator/qwt/examples/radio/subdir.mk
-include visualnoc/integrator/qwt/examples/oscilloscope/subdir.mk
-include visualnoc/integrator/qwt/examples/legends/subdir.mk
-include visualnoc/integrator/qwt/examples/itemeditor/subdir.mk
-include visualnoc/integrator/qwt/examples/friedberg/subdir.mk
-include visualnoc/integrator/qwt/examples/event_filter/subdir.mk
-include visualnoc/integrator/qwt/examples/distrowatch/subdir.mk
-include visualnoc/integrator/qwt/examples/dials/subdir.mk
-include visualnoc/integrator/qwt/examples/curvdemo1/subdir.mk
-include visualnoc/integrator/qwt/examples/cpuplot/subdir.mk
-include visualnoc/integrator/qwt/examples/controls/subdir.mk
-include visualnoc/integrator/qwt/examples/bode/subdir.mk
-include visualnoc/integrator/qwt/examples/barchart/subdir.mk
-include visualnoc/integrator/qwt/examples/animation/subdir.mk
-include visualnoc/integrator/qwt/designer/resources/subdir.mk
-include visualnoc/integrator/qwt/designer/obj/subdir.mk
-include visualnoc/integrator/qwt/designer/moc/subdir.mk
-include visualnoc/integrator/qwt/designer/subdir.mk
-include visualnoc/integrator/obj/subdir.mk
-include visualnoc/integrator/modelsview/subdir.mk
-include visualnoc/integrator/moc/subdir.mk
-include visualnoc/integrator/define/subdir.mk
-include visualnoc/integrator/subdir.mk
-include visualnoc/esynet/orion_power_beta_mar2003/power/subdir.mk
-include visualnoc/esynet/subdir.mk
-include trapgen/io_less/subdir.mk
-include trapgen/subdir.mk
-include tests/src/subdir.mk
-include testmpi/fft64/subdir.mk
-include testmpi/subdir.mk
-include mcpat/obj_dbg/subdir.mk
-include mcpat/cacti/obj_opt/subdir.mk
-include mcpat/cacti/subdir.mk
-include mcpat/subdir.mk
-include makedepend/subdir.mk
-include libexo/subdir.mk
-include hotspot/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.managedbuilder.core/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: svn2tt

# Tool invocations
svn2tt: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "svn2tt" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) svn2tt
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
