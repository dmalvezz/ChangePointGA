################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/model/escorpio17.c \
../src/model/londontrack18.c \
../src/model/map.c \
../src/model/simulation.c 

OBJS += \
./src/model/escorpio17.o \
./src/model/londontrack18.o \
./src/model/map.o \
./src/model/simulation.o 

C_DEPS += \
./src/model/escorpio17.d \
./src/model/londontrack18.d \
./src/model/map.d \
./src/model/simulation.d 


# Each subdirectory must supply rules for building sources it contributes
src/model/%.o: ../src/model/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	mpicc -fopenmp -O1 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


