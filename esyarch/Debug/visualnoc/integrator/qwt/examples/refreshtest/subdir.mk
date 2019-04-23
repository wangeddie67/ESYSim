################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/examples/refreshtest/circularbuffer.cpp \
../visualnoc/integrator/qwt/examples/refreshtest/main.cpp \
../visualnoc/integrator/qwt/examples/refreshtest/mainwindow.cpp \
../visualnoc/integrator/qwt/examples/refreshtest/panel.cpp \
../visualnoc/integrator/qwt/examples/refreshtest/plot.cpp 

OBJS += \
./visualnoc/integrator/qwt/examples/refreshtest/circularbuffer.o \
./visualnoc/integrator/qwt/examples/refreshtest/main.o \
./visualnoc/integrator/qwt/examples/refreshtest/mainwindow.o \
./visualnoc/integrator/qwt/examples/refreshtest/panel.o \
./visualnoc/integrator/qwt/examples/refreshtest/plot.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/examples/refreshtest/circularbuffer.d \
./visualnoc/integrator/qwt/examples/refreshtest/main.d \
./visualnoc/integrator/qwt/examples/refreshtest/mainwindow.d \
./visualnoc/integrator/qwt/examples/refreshtest/panel.d \
./visualnoc/integrator/qwt/examples/refreshtest/plot.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/examples/refreshtest/%.o: ../visualnoc/integrator/qwt/examples/refreshtest/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


