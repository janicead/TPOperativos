################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/compactador.c \
../src/fs.c \
../src/lfs.c \
../src/varios.c 

OBJS += \
./src/compactador.o \
./src/fs.o \
./src/lfs.o \
./src/varios.o 

C_DEPS += \
./src/compactador.d \
./src/fs.d \
./src/lfs.d \
./src/varios.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-1c-BEFGN/bibliotecaFunciones" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


