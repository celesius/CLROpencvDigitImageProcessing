################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/IFVRStereoCamera/IFVRStereoCamera.cpp \
../src/IFVRStereoCamera/video_device.cpp 

C_SRCS += \
../src/IFVRStereoCamera/v4l2grab.c 

OBJS += \
./src/IFVRStereoCamera/IFVRStereoCamera.o \
./src/IFVRStereoCamera/v4l2grab.o \
./src/IFVRStereoCamera/video_device.o 

CPP_DEPS += \
./src/IFVRStereoCamera/IFVRStereoCamera.d \
./src/IFVRStereoCamera/video_device.d 

C_DEPS += \
./src/IFVRStereoCamera/v4l2grab.d 


# Each subdirectory must supply rules for building sources it contributes
src/IFVRStereoCamera/%.o: ../src/IFVRStereoCamera/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/IFVRStereoCamera/%.o: ../src/IFVRStereoCamera/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


