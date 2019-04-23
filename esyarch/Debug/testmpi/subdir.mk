################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../testmpi/API.o \
../testmpi/mainmpi.o 

C_SRCS += \
../testmpi/mainmpi.c 

OBJS += \
./testmpi/mainmpi.o 

C_DEPS += \
./testmpi/mainmpi.d 


# Each subdirectory must supply rules for building sources it contributes
testmpi/%.o: ../testmpi/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


