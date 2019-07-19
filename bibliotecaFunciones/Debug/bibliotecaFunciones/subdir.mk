################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bibliotecaFunciones/consola.c \
../bibliotecaFunciones/lfsProtocolos.c \
../bibliotecaFunciones/lfsSerializacion.c \
../bibliotecaFunciones/lfsVarios.c \
../bibliotecaFunciones/sockets.c \
../bibliotecaFunciones/usoVariado.c 

O_SRCS += \
../bibliotecaFunciones/consola.o \
../bibliotecaFunciones/lfsProtocolos.o \
../bibliotecaFunciones/lfsSerializacion.o \
../bibliotecaFunciones/lfsVarios.o \
../bibliotecaFunciones/sockets.o \
../bibliotecaFunciones/usoVariado.o 

OBJS += \
./bibliotecaFunciones/consola.o \
./bibliotecaFunciones/lfsProtocolos.o \
./bibliotecaFunciones/lfsSerializacion.o \
./bibliotecaFunciones/lfsVarios.o \
./bibliotecaFunciones/sockets.o \
./bibliotecaFunciones/usoVariado.o 

C_DEPS += \
./bibliotecaFunciones/consola.d \
./bibliotecaFunciones/lfsProtocolos.d \
./bibliotecaFunciones/lfsSerializacion.d \
./bibliotecaFunciones/lfsVarios.d \
./bibliotecaFunciones/sockets.d \
./bibliotecaFunciones/usoVariado.d 


# Each subdirectory must supply rules for building sources it contributes
bibliotecaFunciones/%.o: ../bibliotecaFunciones/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


