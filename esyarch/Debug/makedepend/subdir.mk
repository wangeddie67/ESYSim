################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../makedepend/cppsetup.o \
../makedepend/include.o \
../makedepend/main.o \
../makedepend/parse.o \
../makedepend/pr.o 

C_SRCS += \
../makedepend/cppsetup.c \
../makedepend/include.c \
../makedepend/main.c \
../makedepend/parse.c \
../makedepend/pr.c 

OBJS += \
./makedepend/cppsetup.o \
./makedepend/include.o \
./makedepend/main.o \
./makedepend/parse.o \
./makedepend/pr.o 

C_DEPS += \
./makedepend/cppsetup.d \
./makedepend/include.d \
./makedepend/main.d \
./makedepend/parse.d \
./makedepend/pr.d 


# Each subdirectory must supply rules for building sources it contributes
makedepend/%.o: ../makedepend/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


