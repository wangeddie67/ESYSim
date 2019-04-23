################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../visualnoc/esynet/esynet_config.o \
../visualnoc/esynet/esynet_ecc_unit.o \
../visualnoc/esynet/esynet_fault_unit.o \
../visualnoc/esynet/esynet_flit.o \
../visualnoc/esynet/esynet_foundation.o \
../visualnoc/esynet/esynet_interface.o \
../visualnoc/esynet/esynet_mess_event.o \
../visualnoc/esynet/esynet_mess_queue.o \
../visualnoc/esynet/esynet_pe_unit.o \
../visualnoc/esynet/esynet_port_unit.o \
../visualnoc/esynet/esynet_random_unit.o \
../visualnoc/esynet/esynet_router_fault.o \
../visualnoc/esynet/esynet_router_power.o \
../visualnoc/esynet/esynet_router_unit.o \
../visualnoc/esynet/esynet_routing_frr.o \
../visualnoc/esynet/esynet_routing_ftlr.o \
../visualnoc/esynet/esynet_routing_gradient.o \
../visualnoc/esynet/esynet_routing_hipfar.o \
../visualnoc/esynet/esynet_routing_rescuer.o \
../visualnoc/esynet/esynet_routing_xy.o \
../visualnoc/esynet/esynet_statistics.o 

CC_SRCS += \
../visualnoc/esynet/esynet_config.cc \
../visualnoc/esynet/esynet_ecc_unit.cc \
../visualnoc/esynet/esynet_fault_unit.cc \
../visualnoc/esynet/esynet_flit.cc \
../visualnoc/esynet/esynet_foundation.cc \
../visualnoc/esynet/esynet_interface.cc \
../visualnoc/esynet/esynet_main.cc \
../visualnoc/esynet/esynet_mess_event.cc \
../visualnoc/esynet/esynet_mess_queue.cc \
../visualnoc/esynet/esynet_path.cc \
../visualnoc/esynet/esynet_pe_unit.cc \
../visualnoc/esynet/esynet_port_unit.cc \
../visualnoc/esynet/esynet_random_unit.cc \
../visualnoc/esynet/esynet_router_fault.cc \
../visualnoc/esynet/esynet_router_power.cc \
../visualnoc/esynet/esynet_router_unit.cc \
../visualnoc/esynet/esynet_routing_fon.cc \
../visualnoc/esynet/esynet_routing_frr.cc \
../visualnoc/esynet/esynet_routing_ftlr.cc \
../visualnoc/esynet/esynet_routing_gradient.cc \
../visualnoc/esynet/esynet_routing_hipfar.cc \
../visualnoc/esynet/esynet_routing_rescuer.cc \
../visualnoc/esynet/esynet_routing_xy.cc \
../visualnoc/esynet/esynet_statistics.cc 

OBJS += \
./visualnoc/esynet/esynet_config.o \
./visualnoc/esynet/esynet_ecc_unit.o \
./visualnoc/esynet/esynet_fault_unit.o \
./visualnoc/esynet/esynet_flit.o \
./visualnoc/esynet/esynet_foundation.o \
./visualnoc/esynet/esynet_interface.o \
./visualnoc/esynet/esynet_main.o \
./visualnoc/esynet/esynet_mess_event.o \
./visualnoc/esynet/esynet_mess_queue.o \
./visualnoc/esynet/esynet_path.o \
./visualnoc/esynet/esynet_pe_unit.o \
./visualnoc/esynet/esynet_port_unit.o \
./visualnoc/esynet/esynet_random_unit.o \
./visualnoc/esynet/esynet_router_fault.o \
./visualnoc/esynet/esynet_router_power.o \
./visualnoc/esynet/esynet_router_unit.o \
./visualnoc/esynet/esynet_routing_fon.o \
./visualnoc/esynet/esynet_routing_frr.o \
./visualnoc/esynet/esynet_routing_ftlr.o \
./visualnoc/esynet/esynet_routing_gradient.o \
./visualnoc/esynet/esynet_routing_hipfar.o \
./visualnoc/esynet/esynet_routing_rescuer.o \
./visualnoc/esynet/esynet_routing_xy.o \
./visualnoc/esynet/esynet_statistics.o 

CC_DEPS += \
./visualnoc/esynet/esynet_config.d \
./visualnoc/esynet/esynet_ecc_unit.d \
./visualnoc/esynet/esynet_fault_unit.d \
./visualnoc/esynet/esynet_flit.d \
./visualnoc/esynet/esynet_foundation.d \
./visualnoc/esynet/esynet_interface.d \
./visualnoc/esynet/esynet_main.d \
./visualnoc/esynet/esynet_mess_event.d \
./visualnoc/esynet/esynet_mess_queue.d \
./visualnoc/esynet/esynet_path.d \
./visualnoc/esynet/esynet_pe_unit.d \
./visualnoc/esynet/esynet_port_unit.d \
./visualnoc/esynet/esynet_random_unit.d \
./visualnoc/esynet/esynet_router_fault.d \
./visualnoc/esynet/esynet_router_power.d \
./visualnoc/esynet/esynet_router_unit.d \
./visualnoc/esynet/esynet_routing_fon.d \
./visualnoc/esynet/esynet_routing_frr.d \
./visualnoc/esynet/esynet_routing_ftlr.d \
./visualnoc/esynet/esynet_routing_gradient.d \
./visualnoc/esynet/esynet_routing_hipfar.d \
./visualnoc/esynet/esynet_routing_rescuer.d \
./visualnoc/esynet/esynet_routing_xy.d \
./visualnoc/esynet/esynet_statistics.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/esynet/%.o: ../visualnoc/esynet/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


