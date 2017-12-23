################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ga/operators/crossover.c \
../src/ga/operators/fitness.c \
../src/ga/operators/mutation.c \
../src/ga/operators/selection.c 

OBJS += \
./src/ga/operators/crossover.o \
./src/ga/operators/fitness.o \
./src/ga/operators/mutation.o \
./src/ga/operators/selection.o 

C_DEPS += \
./src/ga/operators/crossover.d \
./src/ga/operators/fitness.d \
./src/ga/operators/mutation.d \
./src/ga/operators/selection.d 


# Each subdirectory must supply rules for building sources it contributes
src/ga/operators/%.o: ../src/ga/operators/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -fopenmp -O1 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


