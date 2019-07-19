################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lissandra-File-System/LFS.c \
../Lissandra-File-System/LFS_commons.c \
../Lissandra-File-System/configLFS.c \
../Lissandra-File-System/consolaLFS.c 

OBJS += \
./Lissandra-File-System/LFS.o \
./Lissandra-File-System/LFS_commons.o \
./Lissandra-File-System/configLFS.o \
./Lissandra-File-System/consolaLFS.o 

C_DEPS += \
./Lissandra-File-System/LFS.d \
./Lissandra-File-System/LFS_commons.d \
./Lissandra-File-System/configLFS.d \
./Lissandra-File-System/consolaLFS.d 


# Each subdirectory must supply rules for building sources it contributes
Lissandra-File-System/%.o: ../Lissandra-File-System/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


