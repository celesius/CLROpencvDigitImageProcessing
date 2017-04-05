################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CLROpencvAndDigitImageProcessing.cpp \
../src/CLRProcessingCommon.cpp \
../src/CLRReadAndCombShow.cpp 

OBJS += \
./src/CLROpencvAndDigitImageProcessing.o \
./src/CLRProcessingCommon.o \
./src/CLRReadAndCombShow.o 

CPP_DEPS += \
./src/CLROpencvAndDigitImageProcessing.d \
./src/CLRProcessingCommon.d \
./src/CLRReadAndCombShow.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/home/clover/workspace/IFVRBlob/ -I/home/clover/workspace/IFVRDepthGround/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


