################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CLRCVHelper/CLRCVHelper.cpp 

OBJS += \
./src/CLRCVHelper/CLRCVHelper.o 

CPP_DEPS += \
./src/CLRCVHelper/CLRCVHelper.d 


# Each subdirectory must supply rules for building sources it contributes
src/CLRCVHelper/%.o: ../src/CLRCVHelper/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


