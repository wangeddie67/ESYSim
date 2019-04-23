################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/examples/oscilloscope/curvedata.cpp \
../visualnoc/integrator/qwt/examples/oscilloscope/knob.cpp \
../visualnoc/integrator/qwt/examples/oscilloscope/main.cpp \
../visualnoc/integrator/qwt/examples/oscilloscope/mainwindow.cpp \
../visualnoc/integrator/qwt/examples/oscilloscope/plot.cpp \
../visualnoc/integrator/qwt/examples/oscilloscope/samplingthread.cpp \
../visualnoc/integrator/qwt/examples/oscilloscope/signaldata.cpp \
../visualnoc/integrator/qwt/examples/oscilloscope/wheelbox.cpp 

OBJS += \
./visualnoc/integrator/qwt/examples/oscilloscope/curvedata.o \
./visualnoc/integrator/qwt/examples/oscilloscope/knob.o \
./visualnoc/integrator/qwt/examples/oscilloscope/main.o \
./visualnoc/integrator/qwt/examples/oscilloscope/mainwindow.o \
./visualnoc/integrator/qwt/examples/oscilloscope/plot.o \
./visualnoc/integrator/qwt/examples/oscilloscope/samplingthread.o \
./visualnoc/integrator/qwt/examples/oscilloscope/signaldata.o \
./visualnoc/integrator/qwt/examples/oscilloscope/wheelbox.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/examples/oscilloscope/curvedata.d \
./visualnoc/integrator/qwt/examples/oscilloscope/knob.d \
./visualnoc/integrator/qwt/examples/oscilloscope/main.d \
./visualnoc/integrator/qwt/examples/oscilloscope/mainwindow.d \
./visualnoc/integrator/qwt/examples/oscilloscope/plot.d \
./visualnoc/integrator/qwt/examples/oscilloscope/samplingthread.d \
./visualnoc/integrator/qwt/examples/oscilloscope/signaldata.d \
./visualnoc/integrator/qwt/examples/oscilloscope/wheelbox.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/examples/oscilloscope/%.o: ../visualnoc/integrator/qwt/examples/oscilloscope/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


