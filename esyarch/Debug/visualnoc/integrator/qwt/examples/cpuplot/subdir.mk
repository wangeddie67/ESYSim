################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/examples/cpuplot/cpupiemarker.cpp \
../visualnoc/integrator/qwt/examples/cpuplot/cpuplot.cpp \
../visualnoc/integrator/qwt/examples/cpuplot/cpustat.cpp 

OBJS += \
./visualnoc/integrator/qwt/examples/cpuplot/cpupiemarker.o \
./visualnoc/integrator/qwt/examples/cpuplot/cpuplot.o \
./visualnoc/integrator/qwt/examples/cpuplot/cpustat.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/examples/cpuplot/cpupiemarker.d \
./visualnoc/integrator/qwt/examples/cpuplot/cpuplot.d \
./visualnoc/integrator/qwt/examples/cpuplot/cpustat.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/examples/cpuplot/%.o: ../visualnoc/integrator/qwt/examples/cpuplot/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


