################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../configMemoria.c \
../consolaMemoria.c \
../memoria.c \
../segmentacionPaginada.c \
../socketsMemoria.c 

OBJS += \
./configMemoria.o \
./consolaMemoria.o \
./memoria.o \
./segmentacionPaginada.o \
./socketsMemoria.o 

C_DEPS += \
./configMemoria.d \
./consolaMemoria.d \
./memoria.d \
./segmentacionPaginada.d \
./socketsMemoria.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-1c-BEFGN/bibliotecaFunciones" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


