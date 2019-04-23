################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../trapgen/AES.c \
../trapgen/bcnt.c \
../trapgen/blit.c \
../trapgen/crc.c \
../trapgen/des.c \
../trapgen/dhry.c \
../trapgen/engine.c \
../trapgen/fft.c \
../trapgen/fib.c \
../trapgen/fir2.c \
../trapgen/gamma.c \
../trapgen/hanoi.c \
../trapgen/heapsort.c \
../trapgen/jpeg.c \
../trapgen/lms.c \
../trapgen/matrix.c \
../trapgen/mdrAll.c \
../trapgen/pocsag.c \
../trapgen/queens.c \
../trapgen/quicksort.c \
../trapgen/tolstoy.c \
../trapgen/v42.c \
../trapgen/wavelt.c \
../trapgen/whets.c 

OBJS += \
./trapgen/AES.o \
./trapgen/bcnt.o \
./trapgen/blit.o \
./trapgen/crc.o \
./trapgen/des.o \
./trapgen/dhry.o \
./trapgen/engine.o \
./trapgen/fft.o \
./trapgen/fib.o \
./trapgen/fir2.o \
./trapgen/gamma.o \
./trapgen/hanoi.o \
./trapgen/heapsort.o \
./trapgen/jpeg.o \
./trapgen/lms.o \
./trapgen/matrix.o \
./trapgen/mdrAll.o \
./trapgen/pocsag.o \
./trapgen/queens.o \
./trapgen/quicksort.o \
./trapgen/tolstoy.o \
./trapgen/v42.o \
./trapgen/wavelt.o \
./trapgen/whets.o 

C_DEPS += \
./trapgen/AES.d \
./trapgen/bcnt.d \
./trapgen/blit.d \
./trapgen/crc.d \
./trapgen/des.d \
./trapgen/dhry.d \
./trapgen/engine.d \
./trapgen/fft.d \
./trapgen/fib.d \
./trapgen/fir2.d \
./trapgen/gamma.d \
./trapgen/hanoi.d \
./trapgen/heapsort.d \
./trapgen/jpeg.d \
./trapgen/lms.d \
./trapgen/matrix.d \
./trapgen/mdrAll.d \
./trapgen/pocsag.d \
./trapgen/queens.d \
./trapgen/quicksort.d \
./trapgen/tolstoy.d \
./trapgen/v42.d \
./trapgen/wavelt.d \
./trapgen/whets.d 


# Each subdirectory must supply rules for building sources it contributes
trapgen/%.o: ../trapgen/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


