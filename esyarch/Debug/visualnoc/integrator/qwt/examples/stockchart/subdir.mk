################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/examples/stockchart/griditem.cpp \
../visualnoc/integrator/qwt/examples/stockchart/legend.cpp \
../visualnoc/integrator/qwt/examples/stockchart/main.cpp \
../visualnoc/integrator/qwt/examples/stockchart/plot.cpp \
../visualnoc/integrator/qwt/examples/stockchart/quotefactory.cpp 

OBJS += \
./visualnoc/integrator/qwt/examples/stockchart/griditem.o \
./visualnoc/integrator/qwt/examples/stockchart/legend.o \
./visualnoc/integrator/qwt/examples/stockchart/main.o \
./visualnoc/integrator/qwt/examples/stockchart/plot.o \
./visualnoc/integrator/qwt/examples/stockchart/quotefactory.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/examples/stockchart/griditem.d \
./visualnoc/integrator/qwt/examples/stockchart/legend.d \
./visualnoc/integrator/qwt/examples/stockchart/main.d \
./visualnoc/integrator/qwt/examples/stockchart/plot.d \
./visualnoc/integrator/qwt/examples/stockchart/quotefactory.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/examples/stockchart/%.o: ../visualnoc/integrator/qwt/examples/stockchart/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


