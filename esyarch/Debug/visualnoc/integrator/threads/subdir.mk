################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../visualnoc/integrator/threads/analyse_benchmark_thread.cc \
../visualnoc/integrator/threads/analyse_eventtrace_thread.cc \
../visualnoc/integrator/threads/analyse_pathtrace_thread.cc 

OBJS += \
./visualnoc/integrator/threads/analyse_benchmark_thread.o \
./visualnoc/integrator/threads/analyse_eventtrace_thread.o \
./visualnoc/integrator/threads/analyse_pathtrace_thread.o 

CC_DEPS += \
./visualnoc/integrator/threads/analyse_benchmark_thread.d \
./visualnoc/integrator/threads/analyse_eventtrace_thread.d \
./visualnoc/integrator/threads/analyse_pathtrace_thread.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/threads/%.o: ../visualnoc/integrator/threads/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


