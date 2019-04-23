################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/examples/distrowatch/barchart.cpp \
../visualnoc/integrator/qwt/examples/distrowatch/main.cpp 

OBJS += \
./visualnoc/integrator/qwt/examples/distrowatch/barchart.o \
./visualnoc/integrator/qwt/examples/distrowatch/main.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/examples/distrowatch/barchart.d \
./visualnoc/integrator/qwt/examples/distrowatch/main.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/examples/distrowatch/%.o: ../visualnoc/integrator/qwt/examples/distrowatch/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


