################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../visualnoc/tinyxml/tinystr.o \
../visualnoc/tinyxml/tinyxml.o \
../visualnoc/tinyxml/tinyxmlerror.o \
../visualnoc/tinyxml/tinyxmlparser.o 

CPP_SRCS += \
../visualnoc/tinyxml/tinystr.cpp \
../visualnoc/tinyxml/tinyxml.cpp \
../visualnoc/tinyxml/tinyxmlerror.cpp \
../visualnoc/tinyxml/tinyxmlparser.cpp \
../visualnoc/tinyxml/xmltest.cpp 

OBJS += \
./visualnoc/tinyxml/tinystr.o \
./visualnoc/tinyxml/tinyxml.o \
./visualnoc/tinyxml/tinyxmlerror.o \
./visualnoc/tinyxml/tinyxmlparser.o \
./visualnoc/tinyxml/xmltest.o 

CPP_DEPS += \
./visualnoc/tinyxml/tinystr.d \
./visualnoc/tinyxml/tinyxml.d \
./visualnoc/tinyxml/tinyxmlerror.d \
./visualnoc/tinyxml/tinyxmlparser.d \
./visualnoc/tinyxml/xmltest.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/tinyxml/%.o: ../visualnoc/tinyxml/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


