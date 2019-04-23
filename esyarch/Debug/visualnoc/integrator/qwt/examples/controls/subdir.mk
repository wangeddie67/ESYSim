################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/examples/controls/dialbox.cpp \
../visualnoc/integrator/qwt/examples/controls/dialtab.cpp \
../visualnoc/integrator/qwt/examples/controls/knobbox.cpp \
../visualnoc/integrator/qwt/examples/controls/knobtab.cpp \
../visualnoc/integrator/qwt/examples/controls/main.cpp \
../visualnoc/integrator/qwt/examples/controls/sliderbox.cpp \
../visualnoc/integrator/qwt/examples/controls/slidertab.cpp \
../visualnoc/integrator/qwt/examples/controls/wheelbox.cpp \
../visualnoc/integrator/qwt/examples/controls/wheeltab.cpp 

OBJS += \
./visualnoc/integrator/qwt/examples/controls/dialbox.o \
./visualnoc/integrator/qwt/examples/controls/dialtab.o \
./visualnoc/integrator/qwt/examples/controls/knobbox.o \
./visualnoc/integrator/qwt/examples/controls/knobtab.o \
./visualnoc/integrator/qwt/examples/controls/main.o \
./visualnoc/integrator/qwt/examples/controls/sliderbox.o \
./visualnoc/integrator/qwt/examples/controls/slidertab.o \
./visualnoc/integrator/qwt/examples/controls/wheelbox.o \
./visualnoc/integrator/qwt/examples/controls/wheeltab.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/examples/controls/dialbox.d \
./visualnoc/integrator/qwt/examples/controls/dialtab.d \
./visualnoc/integrator/qwt/examples/controls/knobbox.d \
./visualnoc/integrator/qwt/examples/controls/knobtab.d \
./visualnoc/integrator/qwt/examples/controls/main.d \
./visualnoc/integrator/qwt/examples/controls/sliderbox.d \
./visualnoc/integrator/qwt/examples/controls/slidertab.d \
./visualnoc/integrator/qwt/examples/controls/wheelbox.d \
./visualnoc/integrator/qwt/examples/controls/wheeltab.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/examples/controls/%.o: ../visualnoc/integrator/qwt/examples/controls/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


