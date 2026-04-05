################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ESC/esc.c 

OBJS += \
./Core/Src/ESC/esc.o 

C_DEPS += \
./Core/Src/ESC/esc.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/ESC/%.o Core/Src/ESC/%.su Core/Src/ESC/%.cyclo: ../Core/Src/ESC/%.c Core/Src/ESC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-ESC

clean-Core-2f-Src-2f-ESC:
	-$(RM) ./Core/Src/ESC/esc.cyclo ./Core/Src/ESC/esc.d ./Core/Src/ESC/esc.o ./Core/Src/ESC/esc.su

.PHONY: clean-Core-2f-Src-2f-ESC

