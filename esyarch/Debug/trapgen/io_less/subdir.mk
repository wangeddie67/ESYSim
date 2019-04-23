################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../trapgen/io_less/AES.c \
../trapgen/io_less/bcnt.c \
../trapgen/io_less/blit.c \
../trapgen/io_less/crc.c \
../trapgen/io_less/des.c \
../trapgen/io_less/engine.c \
../trapgen/io_less/fib.c \
../trapgen/io_less/fir2.c \
../trapgen/io_less/hanoi.c \
../trapgen/io_less/heapsort.c \
../trapgen/io_less/matrix.c \
../trapgen/io_less/pocsag.c \
../trapgen/io_less/queens.c \
../trapgen/io_less/quicksort.c \
../trapgen/io_less/validate.c 

OBJS += \
./trapgen/io_less/AES.o \
./trapgen/io_less/bcnt.o \
./trapgen/io_less/blit.o \
./trapgen/io_less/crc.o \
./trapgen/io_less/des.o \
./trapgen/io_less/engine.o \
./trapgen/io_less/fib.o \
./trapgen/io_less/fir2.o \
./trapgen/io_less/hanoi.o \
./trapgen/io_less/heapsort.o \
./trapgen/io_less/matrix.o \
./trapgen/io_less/pocsag.o \
./trapgen/io_less/queens.o \
./trapgen/io_less/quicksort.o \
./trapgen/io_less/validate.o 

C_DEPS += \
./trapgen/io_less/AES.d \
./trapgen/io_less/bcnt.d \
./trapgen/io_less/blit.d \
./trapgen/io_less/crc.d \
./trapgen/io_less/des.d \
./trapgen/io_less/engine.d \
./trapgen/io_less/fib.d \
./trapgen/io_less/fir2.d \
./trapgen/io_less/hanoi.d \
./trapgen/io_less/heapsort.d \
./trapgen/io_less/matrix.d \
./trapgen/io_less/pocsag.d \
./trapgen/io_less/queens.d \
./trapgen/io_less/quicksort.d \
./trapgen/io_less/validate.d 


# Each subdirectory must supply rules for building sources it contributes
trapgen/io_less/%.o: ../trapgen/io_less/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


