################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../libexo/exolex.o \
../libexo/libexo.o 

C_SRCS += \
../libexo/exo-test.c \
../libexo/exolex.c 

CC_SRCS += \
../libexo/libexo.cc 

OBJS += \
./libexo/exo-test.o \
./libexo/exolex.o \
./libexo/libexo.o 

C_DEPS += \
./libexo/exo-test.d \
./libexo/exolex.d 

CC_DEPS += \
./libexo/libexo.d 


# Each subdirectory must supply rules for building sources it contributes
libexo/%.o: ../libexo/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

libexo/%.o: ../libexo/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


