################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cali/DuoCalibrator.cpp \
../src/cali/calibDuo.cpp 

OBJS += \
./src/cali/DuoCalibrator.o \
./src/cali/calibDuo.o 

CPP_DEPS += \
./src/cali/DuoCalibrator.d \
./src/cali/calibDuo.d 


# Each subdirectory must supply rules for building sources it contributes
src/cali/%.o: ../src/cali/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


