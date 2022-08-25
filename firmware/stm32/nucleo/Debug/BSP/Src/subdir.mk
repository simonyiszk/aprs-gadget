################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/Src/fonts.c \
../BSP/Src/fsk.c \
../BSP/Src/ssd1306.c 

OBJS += \
./BSP/Src/fonts.o \
./BSP/Src/fsk.o \
./BSP/Src/ssd1306.o 

C_DEPS += \
./BSP/Src/fonts.d \
./BSP/Src/fsk.d \
./BSP/Src/ssd1306.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/Src/%.o BSP/Src/%.su: ../BSP/Src/%.c BSP/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/pali/Documents/BME/HA5KFU/aprs-gadget/firmware/stm32/nucleo/BSP/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP-2f-Src

clean-BSP-2f-Src:
	-$(RM) ./BSP/Src/fonts.d ./BSP/Src/fonts.o ./BSP/Src/fonts.su ./BSP/Src/fsk.d ./BSP/Src/fsk.o ./BSP/Src/fsk.su ./BSP/Src/ssd1306.d ./BSP/Src/ssd1306.o ./BSP/Src/ssd1306.su

.PHONY: clean-BSP-2f-Src

