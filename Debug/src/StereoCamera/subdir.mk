################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/StereoCamera/CLRStereoCamera.cpp 

OBJS += \
./src/StereoCamera/CLRStereoCamera.o 

CPP_DEPS += \
./src/StereoCamera/CLRStereoCamera.d 


# Each subdirectory must supply rules for building sources it contributes
src/StereoCamera/%.o: ../src/StereoCamera/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


