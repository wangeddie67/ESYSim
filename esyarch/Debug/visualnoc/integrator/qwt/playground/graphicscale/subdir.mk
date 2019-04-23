################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/playground/graphicscale/canvas.cpp \
../visualnoc/integrator/qwt/playground/graphicscale/main.cpp \
../visualnoc/integrator/qwt/playground/graphicscale/mainwindow.cpp 

OBJS += \
./visualnoc/integrator/qwt/playground/graphicscale/canvas.o \
./visualnoc/integrator/qwt/playground/graphicscale/main.o \
./visualnoc/integrator/qwt/playground/graphicscale/mainwindow.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/playground/graphicscale/canvas.d \
./visualnoc/integrator/qwt/playground/graphicscale/main.d \
./visualnoc/integrator/qwt/playground/graphicscale/mainwindow.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/playground/graphicscale/%.o: ../visualnoc/integrator/qwt/playground/graphicscale/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


