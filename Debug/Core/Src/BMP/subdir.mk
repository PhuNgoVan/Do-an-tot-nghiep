################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/BMP/BMP_280.c \
../Core/Src/BMP/app_bmp_280.c 

OBJS += \
./Core/Src/BMP/BMP_280.o \
./Core/Src/BMP/app_bmp_280.o 

C_DEPS += \
./Core/Src/BMP/BMP_280.d \
./Core/Src/BMP/app_bmp_280.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/BMP/%.o Core/Src/BMP/%.su Core/Src/BMP/%.cyclo: ../Core/Src/BMP/%.c Core/Src/BMP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-BMP

clean-Core-2f-Src-2f-BMP:
	-$(RM) ./Core/Src/BMP/BMP_280.cyclo ./Core/Src/BMP/BMP_280.d ./Core/Src/BMP/BMP_280.o ./Core/Src/BMP/BMP_280.su ./Core/Src/BMP/app_bmp_280.cyclo ./Core/Src/BMP/app_bmp_280.d ./Core/Src/BMP/app_bmp_280.o ./Core/Src/BMP/app_bmp_280.su

.PHONY: clean-Core-2f-Src-2f-BMP

