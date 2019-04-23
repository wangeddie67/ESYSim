################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../mcpat/XML_Parse.cc \
../mcpat/array.cc \
../mcpat/basic_components.cc \
../mcpat/core.cc \
../mcpat/interconnect.cc \
../mcpat/iocontrollers.cc \
../mcpat/logic.cc \
../mcpat/main.cc \
../mcpat/mainMc.cc \
../mcpat/memoryctrl.cc \
../mcpat/noc.cc \
../mcpat/processor.cc \
../mcpat/sharedcache.cc \
../mcpat/technology_xeon_core.cc \
../mcpat/xmlParser.cc 

OBJS += \
./mcpat/XML_Parse.o \
./mcpat/array.o \
./mcpat/basic_components.o \
./mcpat/core.o \
./mcpat/interconnect.o \
./mcpat/iocontrollers.o \
./mcpat/logic.o \
./mcpat/main.o \
./mcpat/mainMc.o \
./mcpat/memoryctrl.o \
./mcpat/noc.o \
./mcpat/processor.o \
./mcpat/sharedcache.o \
./mcpat/technology_xeon_core.o \
./mcpat/xmlParser.o 

CC_DEPS += \
./mcpat/XML_Parse.d \
./mcpat/array.d \
./mcpat/basic_components.d \
./mcpat/core.d \
./mcpat/interconnect.d \
./mcpat/iocontrollers.d \
./mcpat/logic.d \
./mcpat/main.d \
./mcpat/mainMc.d \
./mcpat/memoryctrl.d \
./mcpat/noc.d \
./mcpat/processor.d \
./mcpat/sharedcache.d \
./mcpat/technology_xeon_core.d \
./mcpat/xmlParser.d 


# Each subdirectory must supply rules for building sources it contributes
mcpat/%.o: ../mcpat/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


