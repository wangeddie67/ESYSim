################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../testmpi/fft64/radix4.c \
../testmpi/fft64/radix4_input_gen.c \
../testmpi/fft64/radix4_nofile.c \
../testmpi/fft64/radix4_nostdlib.c 

OBJ_SRCS += \
../testmpi/fft64/radix4.obj 

OBJS += \
./testmpi/fft64/radix4.o \
./testmpi/fft64/radix4_input_gen.o \
./testmpi/fft64/radix4_nofile.o \
./testmpi/fft64/radix4_nostdlib.o 

C_DEPS += \
./testmpi/fft64/radix4.d \
./testmpi/fft64/radix4_input_gen.d \
./testmpi/fft64/radix4_nofile.d \
./testmpi/fft64/radix4_nostdlib.d 


# Each subdirectory must supply rules for building sources it contributes
testmpi/fft64/%.o: ../testmpi/fft64/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


