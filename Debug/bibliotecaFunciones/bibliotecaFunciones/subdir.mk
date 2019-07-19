################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bibliotecaFunciones/bibliotecaFunciones/consola.c \
../bibliotecaFunciones/bibliotecaFunciones/lfsSerializacion.c \
../bibliotecaFunciones/bibliotecaFunciones/parser.c \
../bibliotecaFunciones/bibliotecaFunciones/sockets.c \
../bibliotecaFunciones/bibliotecaFunciones/types.c \
../bibliotecaFunciones/bibliotecaFunciones/usoVariado.c 

OBJS += \
./bibliotecaFunciones/bibliotecaFunciones/consola.o \
./bibliotecaFunciones/bibliotecaFunciones/lfsSerializacion.o \
./bibliotecaFunciones/bibliotecaFunciones/parser.o \
./bibliotecaFunciones/bibliotecaFunciones/sockets.o \
./bibliotecaFunciones/bibliotecaFunciones/types.o \
./bibliotecaFunciones/bibliotecaFunciones/usoVariado.o 

C_DEPS += \
./bibliotecaFunciones/bibliotecaFunciones/consola.d \
./bibliotecaFunciones/bibliotecaFunciones/lfsSerializacion.d \
./bibliotecaFunciones/bibliotecaFunciones/parser.d \
./bibliotecaFunciones/bibliotecaFunciones/sockets.d \
./bibliotecaFunciones/bibliotecaFunciones/types.d \
./bibliotecaFunciones/bibliotecaFunciones/usoVariado.d 


# Each subdirectory must supply rules for building sources it contributes
bibliotecaFunciones/bibliotecaFunciones/%.o: ../bibliotecaFunciones/bibliotecaFunciones/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


