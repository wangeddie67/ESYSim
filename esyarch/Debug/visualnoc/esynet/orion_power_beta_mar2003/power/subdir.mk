################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_ALU.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_array_l.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_array_m.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_cam.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_clock.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_misc.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_permu.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_router.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_time.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_util.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_reg_power.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/SIM_router_power.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/dump_para.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/router.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/test_alu.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/test_permu.c \
../visualnoc/esynet/orion_power_beta_mar2003/power/test_router.c 

OBJS += \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_ALU.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_array_l.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_array_m.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_cam.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_clock.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_misc.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_permu.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_router.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_time.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_util.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_reg_power.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_router_power.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/dump_para.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/router.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/test_alu.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/test_permu.o \
./visualnoc/esynet/orion_power_beta_mar2003/power/test_router.o 

C_DEPS += \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_ALU.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_array_l.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_array_m.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_cam.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_clock.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_misc.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_permu.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_router.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_time.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_power_util.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_reg_power.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/SIM_router_power.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/dump_para.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/router.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/test_alu.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/test_permu.d \
./visualnoc/esynet/orion_power_beta_mar2003/power/test_router.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/esynet/orion_power_beta_mar2003/power/%.o: ../visualnoc/esynet/orion_power_beta_mar2003/power/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


