################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/examples/simpleplot/simpleplot.cpp 

OBJS += \
./visualnoc/integrator/qwt/examples/simpleplot/simpleplot.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/examples/simpleplot/simpleplot.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/examples/simpleplot/%.o: ../visualnoc/integrator/qwt/examples/simpleplot/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

