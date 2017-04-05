################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DisparityMask/CLRDisparityMask.cpp 

OBJS += \
./src/DisparityMask/CLRDisparityMask.o 

CPP_DEPS += \
./src/DisparityMask/CLRDisparityMask.d 


# Each subdirectory must supply rules for building sources it contributes
src/DisparityMask/%.o: ../src/DisparityMask/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/home/clover/workspace/IFVRBlob/ -I/home/clover/workspace/IFVRDepthGround/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


