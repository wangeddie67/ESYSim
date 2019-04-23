################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../mcpat/cacti/Ucache.cc \
../mcpat/cacti/arbiter.cc \
../mcpat/cacti/area.cc \
../mcpat/cacti/bank.cc \
../mcpat/cacti/basic_circuit.cc \
../mcpat/cacti/cacti_interface.cc \
../mcpat/cacti/component.cc \
../mcpat/cacti/crossbar.cc \
../mcpat/cacti/decoder.cc \
../mcpat/cacti/htree2.cc \
../mcpat/cacti/io.cc \
../mcpat/cacti/main.cc \
../mcpat/cacti/mat.cc \
../mcpat/cacti/nuca.cc \
../mcpat/cacti/parameter.cc \
../mcpat/cacti/router.cc \
../mcpat/cacti/subarray.cc \
../mcpat/cacti/technology.cc \
../mcpat/cacti/uca.cc \
../mcpat/cacti/wire.cc 

OBJS += \
./mcpat/cacti/Ucache.o \
./mcpat/cacti/arbiter.o \
./mcpat/cacti/area.o \
./mcpat/cacti/bank.o \
./mcpat/cacti/basic_circuit.o \
./mcpat/cacti/cacti_interface.o \
./mcpat/cacti/component.o \
./mcpat/cacti/crossbar.o \
./mcpat/cacti/decoder.o \
./mcpat/cacti/htree2.o \
./mcpat/cacti/io.o \
./mcpat/cacti/main.o \
./mcpat/cacti/mat.o \
./mcpat/cacti/nuca.o \
./mcpat/cacti/parameter.o \
./mcpat/cacti/router.o \
./mcpat/cacti/subarray.o \
./mcpat/cacti/technology.o \
./mcpat/cacti/uca.o \
./mcpat/cacti/wire.o 

CC_DEPS += \
./mcpat/cacti/Ucache.d \
./mcpat/cacti/arbiter.d \
./mcpat/cacti/area.d \
./mcpat/cacti/bank.d \
./mcpat/cacti/basic_circuit.d \
./mcpat/cacti/cacti_interface.d \
./mcpat/cacti/component.d \
./mcpat/cacti/crossbar.d \
./mcpat/cacti/decoder.d \
./mcpat/cacti/htree2.d \
./mcpat/cacti/io.d \
./mcpat/cacti/main.d \
./mcpat/cacti/mat.d \
./mcpat/cacti/nuca.d \
./mcpat/cacti/parameter.d \
./mcpat/cacti/router.d \
./mcpat/cacti/subarray.d \
./mcpat/cacti/technology.d \
./mcpat/cacti/uca.d \
./mcpat/cacti/wire.d 


# Each subdirectory must supply rules for building sources it contributes
mcpat/cacti/%.o: ../mcpat/cacti/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


