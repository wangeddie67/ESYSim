################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../visualnoc/integrator/define/gui_component.cc \
../visualnoc/integrator/define/qt_include.cc \
../visualnoc/integrator/define/router_cfg.cc 

OBJS += \
./visualnoc/integrator/define/gui_component.o \
./visualnoc/integrator/define/qt_include.o \
./visualnoc/integrator/define/router_cfg.o 

CC_DEPS += \
./visualnoc/integrator/define/gui_component.d \
./visualnoc/integrator/define/qt_include.d \
./visualnoc/integrator/define/router_cfg.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/define/%.o: ../visualnoc/integrator/define/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


