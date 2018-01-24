################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/process/masterProcess.c \
../src/process/slaveProcess.c 

OBJS += \
./src/process/masterProcess.o \
./src/process/slaveProcess.o 

C_DEPS += \
./src/process/masterProcess.d \
./src/process/slaveProcess.d 


# Each subdirectory must supply rules for building sources it contributes
src/process/%.o: ../src/process/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	mpicc -fopenmp -O1 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


