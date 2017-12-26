################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ga/ga.c \
../src/ga/generation.c \
../src/ga/stategy.c 

OBJS += \
./src/ga/ga.o \
./src/ga/generation.o \
./src/ga/stategy.o 

C_DEPS += \
./src/ga/ga.d \
./src/ga/generation.d \
./src/ga/stategy.d 


# Each subdirectory must supply rules for building sources it contributes
src/ga/%.o: ../src/ga/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -fopenmp -O1 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


