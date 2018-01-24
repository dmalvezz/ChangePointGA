################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/window/commands.c \
../src/window/console.c \
../src/window/window.c 

OBJS += \
./src/window/commands.o \
./src/window/console.o \
./src/window/window.o 

C_DEPS += \
./src/window/commands.d \
./src/window/console.d \
./src/window/window.d 


# Each subdirectory must supply rules for building sources it contributes
src/window/%.o: ../src/window/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	mpicc -fopenmp -O1 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


