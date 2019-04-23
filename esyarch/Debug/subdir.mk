################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../MPITable.o \
../Noc.o \
../Powermc.o \
../Simulation.o \
../Statistics.o \
../alpha.o \
../core.o \
../endian.o \
../main.o \
../misc.o \
../network.o \
../node.o \
../pisa.o \
../resource.o 

CPP_SRCS += \
../MPITrans.cpp 

CC_SRCS += \
../MPIQueue.cc \
../MPITable.cc \
../Noc.cc \
../Powermc.cc \
../Simulation.cc \
../Statistics.cc \
../alpha.cc \
../core.cc \
../endian.cc \
../eval.cc \
../main.cc \
../misc.cc \
../network.cc \
../node.cc \
../pisa.cc \
../resource.cc 

OBJS += \
./MPIQueue.o \
./MPITable.o \
./MPITrans.o \
./Noc.o \
./Powermc.o \
./Simulation.o \
./Statistics.o \
./alpha.o \
./core.o \
./endian.o \
./eval.o \
./main.o \
./misc.o \
./network.o \
./node.o \
./pisa.o \
./resource.o 

CC_DEPS += \
./MPIQueue.d \
./MPITable.d \
./Noc.d \
./Powermc.d \
./Simulation.d \
./Statistics.d \
./alpha.d \
./core.d \
./endian.d \
./eval.d \
./main.d \
./misc.d \
./network.d \
./node.d \
./pisa.d \
./resource.d 

CPP_DEPS += \
./MPITrans.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


