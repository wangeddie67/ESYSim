################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/qtparallel/main.cpp \
../visualnoc/qtparallel/parallelclass.cpp \
../visualnoc/qtparallel/processclass.cpp 

OBJS += \
./visualnoc/qtparallel/main.o \
./visualnoc/qtparallel/parallelclass.o \
./visualnoc/qtparallel/processclass.o 

CPP_DEPS += \
./visualnoc/qtparallel/main.d \
./visualnoc/qtparallel/parallelclass.d \
./visualnoc/qtparallel/processclass.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/qtparallel/%.o: ../visualnoc/qtparallel/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


