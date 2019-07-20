################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Kernel.c \
../configKernel.c \
../consolaKernel.c \
../kernel_commons.c \
../parser.c \
../planificador.c \
../socketsKernel.c 

OBJS += \
./Kernel.o \
./configKernel.o \
./consolaKernel.o \
./kernel_commons.o \
./parser.o \
./planificador.o \
./socketsKernel.o 

C_DEPS += \
./Kernel.d \
./configKernel.d \
./consolaKernel.d \
./kernel_commons.d \
./parser.d \
./planificador.d \
./socketsKernel.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-1c-BEFGN/bibliotecaFunciones" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


