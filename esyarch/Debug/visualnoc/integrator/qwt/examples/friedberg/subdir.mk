################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/examples/friedberg/friedberg2007.cpp \
../visualnoc/integrator/qwt/examples/friedberg/main.cpp \
../visualnoc/integrator/qwt/examples/friedberg/plot.cpp 

OBJS += \
./visualnoc/integrator/qwt/examples/friedberg/friedberg2007.o \
./visualnoc/integrator/qwt/examples/friedberg/main.o \
./visualnoc/integrator/qwt/examples/friedberg/plot.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/examples/friedberg/friedberg2007.d \
./visualnoc/integrator/qwt/examples/friedberg/main.d \
./visualnoc/integrator/qwt/examples/friedberg/plot.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/examples/friedberg/%.o: ../visualnoc/integrator/qwt/examples/friedberg/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


