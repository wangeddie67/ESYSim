################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../visualnoc/integrator/qwt/textengines/mathml/qwt_mathml_text_engine.cpp \
../visualnoc/integrator/qwt/textengines/mathml/qwt_mml_document.cpp 

OBJS += \
./visualnoc/integrator/qwt/textengines/mathml/qwt_mathml_text_engine.o \
./visualnoc/integrator/qwt/textengines/mathml/qwt_mml_document.o 

CPP_DEPS += \
./visualnoc/integrator/qwt/textengines/mathml/qwt_mathml_text_engine.d \
./visualnoc/integrator/qwt/textengines/mathml/qwt_mml_document.d 


# Each subdirectory must supply rules for building sources it contributes
visualnoc/integrator/qwt/textengines/mathml/%.o: ../visualnoc/integrator/qwt/textengines/mathml/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


