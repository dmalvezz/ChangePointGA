################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ga/datatype/gaMPIDatatype.c 

OBJS += \
./src/ga/datatype/gaMPIDatatype.o 

C_DEPS += \
./src/ga/datatype/gaMPIDatatype.d 


# Each subdirectory must supply rules for building sources it contributes
src/ga/datatype/%.o: ../src/ga/datatype/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	mpicc -fopenmp -O1 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


