################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../visualnoc/integrator/integrator.cc \
../visualnoc/integrator/main.cc 

OBJS += \
./visualnoc/integrator/integrator.o \
./visualnoc/integrator/main.o 

CC_DEPS += \
./visualnoc/integrator/integrator.d \
./visualnoc/integrator/main.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/%.o: ../visualnoc/integrator/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


