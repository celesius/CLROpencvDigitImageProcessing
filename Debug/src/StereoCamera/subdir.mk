################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/StereoCamera/CLRStereoCamera.cpp \
../src/StereoCamera/cpptest.cpp 

OBJS += \
./src/StereoCamera/CLRStereoCamera.o \
./src/StereoCamera/cpptest.o 

CPP_DEPS += \
./src/StereoCamera/CLRStereoCamera.d \
./src/StereoCamera/cpptest.d 


# Each subdirectory must supply rules for building sources it contributes
src/StereoCamera/%.o: ../src/StereoCamera/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/home/clover/workspace/IFVRBlob/ -I/home/clover/workspace/IFVRDepthGround/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


