################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include bibliotecaFunciones/bibliotecaFunciones/subdir.mk
-include Memoria/subdir.mk
-include Lissandra-File-System/subdir.mk
-include Kernel/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: tp-2019-1c-BEFGN

# Tool invocations
tp-2019-1c-BEFGN: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "tp-2019-1c-BEFGN" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) tp-2019-1c-BEFGN
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
