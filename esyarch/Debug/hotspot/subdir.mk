################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hotspot/RCutil.c \
../hotspot/flp.c \
../hotspot/flp_desc.c \
../hotspot/hotfloorplan.c \
../hotspot/hotspot.c \
../hotspot/npe.c \
../hotspot/package.c \
../hotspot/shape.c \
../hotspot/sim-template.c \
../hotspot/temperature.c \
../hotspot/temperature_block.c \
../hotspot/temperature_grid.c \
../hotspot/temperature_mobile.c \
../hotspot/util.c \
../hotspot/wire.c 

OBJS += \
./hotspot/RCutil.o \
./hotspot/flp.o \
./hotspot/flp_desc.o \
./hotspot/hotfloorplan.o \
./hotspot/hotspot.o \
./hotspot/npe.o \
./hotspot/package.o \
./hotspot/shape.o \
./hotspot/sim-template.o \
./hotspot/temperature.o \
./hotspot/temperature_block.o \
./hotspot/temperature_grid.o \
./hotspot/temperature_mobile.o \
./hotspot/util.o \
./hotspot/wire.o 

C_DEPS += \
./hotspot/RCutil.d \
./hotspot/flp.d \
./hotspot/flp_desc.d \
./hotspot/hotfloorplan.d \
./hotspot/hotspot.d \
./hotspot/npe.d \
./hotspot/package.d \
./hotspot/shape.d \
./hotspot/sim-template.d \
./hotspot/temperature.d \
./hotspot/temperature_block.d \
./hotspot/temperature_grid.d \
./hotspot/temperature_mobile.d \
./hotspot/util.d \
./hotspot/wire.d 


# Each subdirectory must supply rules for building sources it contributes
hotspot/%.o: ../hotspot/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


