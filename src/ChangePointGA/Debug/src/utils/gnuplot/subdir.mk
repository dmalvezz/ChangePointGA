################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/utils/gnuplot/gnuplot_i.c 

OBJS += \
./src/utils/gnuplot/gnuplot_i.o 

C_DEPS += \
./src/utils/gnuplot/gnuplot_i.d 


# Each subdirectory must supply rules for building sources it contributes
src/utils/gnuplot/%.o: ../src/utils/gnuplot/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -fopenmp -O1 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


