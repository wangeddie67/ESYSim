################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tests/src/anagram.c \
../tests/src/test-fmath.c \
../tests/src/test-llong.c \
../tests/src/test-lswlr.c \
../tests/src/test-math.c \
../tests/src/test-printf.c 

OBJS += \
./tests/src/anagram.o \
./tests/src/test-fmath.o \
./tests/src/test-llong.o \
./tests/src/test-lswlr.o \
./tests/src/test-math.o \
./tests/src/test-printf.o 

C_DEPS += \
./tests/src/anagram.d \
./tests/src/test-fmath.d \
./tests/src/test-llong.d \
./tests/src/test-lswlr.d \
./tests/src/test-math.d \
./tests/src/test-printf.d 


# Each subdirectory must supply rules for building sources it contributes
tests/src/%.o: ../tests/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


