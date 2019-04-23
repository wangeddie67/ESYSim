################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../visualnoc/integrator/subdialogs/analyse_dialog.cc \
../visualnoc/integrator/subdialogs/faults_config_dialog.cc \
../visualnoc/integrator/subdialogs/graphics_config_dialog.cc \
../visualnoc/integrator/subdialogs/link_tool_dialog.cc \
../visualnoc/integrator/subdialogs/noc_dialog.cc \
../visualnoc/integrator/subdialogs/process_dialog.cc \
../visualnoc/integrator/subdialogs/tools_para_dialog.cc 

OBJS += \
./visualnoc/integrator/subdialogs/analyse_dialog.o \
./visualnoc/integrator/subdialogs/faults_config_dialog.o \
./visualnoc/integrator/subdialogs/graphics_config_dialog.o \
./visualnoc/integrator/subdialogs/link_tool_dialog.o \
./visualnoc/integrator/subdialogs/noc_dialog.o \
./visualnoc/integrator/subdialogs/process_dialog.o \
./visualnoc/integrator/subdialogs/tools_para_dialog.o 

CC_DEPS += \
./visualnoc/integrator/subdialogs/analyse_dialog.d \
./visualnoc/integrator/subdialogs/faults_config_dialog.d \
./visualnoc/integrator/subdialogs/graphics_config_dialog.d \
./visualnoc/integrator/subdialogs/link_tool_dialog.d \
./visualnoc/integrator/subdialogs/noc_dialog.d \
./visualnoc/integrator/subdialogs/process_dialog.d \
./visualnoc/integrator/subdialogs/tools_para_dialog.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/subdialogs/%.o: ../visualnoc/integrator/subdialogs/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


