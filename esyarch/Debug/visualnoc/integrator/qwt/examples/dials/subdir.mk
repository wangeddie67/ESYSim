################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/examples/dials/attitude_indicator.cpp \
../visualnoc/integrator/qwt/examples/dials/cockpit_grid.cpp \
../visualnoc/integrator/qwt/examples/dials/compass_grid.cpp \
../visualnoc/integrator/qwt/examples/dials/dials.cpp \
../visualnoc/integrator/qwt/examples/dials/speedo_meter.cpp 

OBJS += \
./visualnoc/integrator/qwt/examples/dials/attitude_indicator.o \
./visualnoc/integrator/qwt/examples/dials/cockpit_grid.o \
./visualnoc/integrator/qwt/examples/dials/compass_grid.o \
./visualnoc/integrator/qwt/examples/dials/dials.o \
./visualnoc/integrator/qwt/examples/dials/speedo_meter.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/examples/dials/attitude_indicator.d \
./visualnoc/integrator/qwt/examples/dials/cockpit_grid.d \
./visualnoc/integrator/qwt/examples/dials/compass_grid.d \
./visualnoc/integrator/qwt/examples/dials/dials.d \
./visualnoc/integrator/qwt/examples/dials/speedo_meter.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/examples/dials/%.o: ../visualnoc/integrator/qwt/examples/dials/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


