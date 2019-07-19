################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Kernel/Kernel.c \
../Kernel/configKernel.c \
../Kernel/consolaKernel.c \
../Kernel/kernel_commons.c \
../Kernel/parser.c \
../Kernel/planificador.c 

OBJS += \
./Kernel/Kernel.o \
./Kernel/configKernel.o \
./Kernel/consolaKernel.o \
./Kernel/kernel_commons.o \
./Kernel/parser.o \
./Kernel/planificador.o 

C_DEPS += \
./Kernel/Kernel.d \
./Kernel/configKernel.d \
./Kernel/consolaKernel.d \
./Kernel/kernel_commons.d \
./Kernel/parser.d \
./Kernel/planificador.d 


# Each subdirectory must supply rules for building sources it contributes
Kernel/%.o: ../Kernel/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


