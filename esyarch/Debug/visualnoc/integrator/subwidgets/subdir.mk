################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../visualnoc/integrator/subwidgets/analyse_fliter_panel.cc \
../visualnoc/integrator/subwidgets/analyse_plot.cc \
../visualnoc/integrator/subwidgets/automation_control_panel.cc \
../visualnoc/integrator/subwidgets/noc_view.cc \
../visualnoc/integrator/subwidgets/port_graphics_item.cc \
../visualnoc/integrator/subwidgets/result_tab_widget.cc \
../visualnoc/integrator/subwidgets/router_graphics_item.cc \
../visualnoc/integrator/subwidgets/tools_config_dialog.cc 

OBJS += \
./visualnoc/integrator/subwidgets/analyse_fliter_panel.o \
./visualnoc/integrator/subwidgets/analyse_plot.o \
./visualnoc/integrator/subwidgets/automation_control_panel.o \
./visualnoc/integrator/subwidgets/noc_view.o \
./visualnoc/integrator/subwidgets/port_graphics_item.o \
./visualnoc/integrator/subwidgets/result_tab_widget.o \
./visualnoc/integrator/subwidgets/router_graphics_item.o \
./visualnoc/integrator/subwidgets/tools_config_dialog.o 

CC_DEPS += \
./visualnoc/integrator/subwidgets/analyse_fliter_panel.d \
./visualnoc/integrator/subwidgets/analyse_plot.d \
./visualnoc/integrator/subwidgets/automation_control_panel.d \
./visualnoc/integrator/subwidgets/noc_view.d \
./visualnoc/integrator/subwidgets/port_graphics_item.d \
./visualnoc/integrator/subwidgets/result_tab_widget.d \
./visualnoc/integrator/subwidgets/router_graphics_item.d \
./visualnoc/integrator/subwidgets/tools_config_dialog.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/subwidgets/%.o: ../visualnoc/integrator/subwidgets/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


