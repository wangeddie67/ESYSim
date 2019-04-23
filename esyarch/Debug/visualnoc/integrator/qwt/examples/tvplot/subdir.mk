################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/examples/tvplot/main.cpp \
../visualnoc/integrator/qwt/examples/tvplot/tvplot.cpp 

OBJS += \
./visualnoc/integrator/qwt/examples/tvplot/main.o \
./visualnoc/integrator/qwt/examples/tvplot/tvplot.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/examples/tvplot/main.d \
./visualnoc/integrator/qwt/examples/tvplot/tvplot.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/examples/tvplot/%.o: ../visualnoc/integrator/qwt/examples/tvplot/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


