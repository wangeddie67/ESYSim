################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../visualnoc/trace2path/main.o 

CC_SRCS += \
../visualnoc/trace2path/main.cc 

OBJS += \
./visualnoc/trace2path/main.o 

CC_DEPS += \
./visualnoc/trace2path/main.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/trace2path/%.o: ../visualnoc/trace2path/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


