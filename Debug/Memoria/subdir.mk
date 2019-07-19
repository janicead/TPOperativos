################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Memoria/configMemoria.c \
../Memoria/consolaMemoria.c \
../Memoria/memoria.c \
../Memoria/segmentacionPaginada.c \
../Memoria/socketsMemoria.c 

OBJS += \
./Memoria/configMemoria.o \
./Memoria/consolaMemoria.o \
./Memoria/memoria.o \
./Memoria/segmentacionPaginada.o \
./Memoria/socketsMemoria.o 

C_DEPS += \
./Memoria/configMemoria.d \
./Memoria/consolaMemoria.d \
./Memoria/memoria.d \
./Memoria/segmentacionPaginada.d \
./Memoria/socketsMemoria.d 


# Each subdirectory must supply rules for building sources it contributes
Memoria/%.o: ../Memoria/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


