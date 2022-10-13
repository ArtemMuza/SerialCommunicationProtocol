cmake_minimum_required(VERSION 3.21.2)

include(arm-none-eabi-gcc.cmake)

project(scp_uart)

enable_language(C ASM)
set(STM32CUBEMX_GENERATED_FILES
        
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_adc.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_adc_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_comp.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_comp_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_crc.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_crc_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cryp.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cryp_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dac.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dac_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dma.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_exti.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_firewall.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ramfunc.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c_ex.c        
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2s.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_irda.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_iwdg.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_lcd.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_lptim.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pcd.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pcd_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rng.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_smartcard.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_smartcard_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_smbus.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_smbus_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_spi.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tsc.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart_ex.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_usart.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_wwdg.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_adc.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_comp.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_crc.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_crs.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_dac.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_dma.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_exti.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_gpio.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_i2c.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_lptim.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_lpuart.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_pwr.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_rcc.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_rng.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_rtc.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_spi.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_tim.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_usart.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_usb.c
        Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_utils.c
        
        Middlewares/Third_Party/FreeRTOS/Source/croutine.c
        Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
        Middlewares/Third_Party/FreeRTOS/Source/list.c 
        Middlewares/Third_Party/FreeRTOS/Source/queue.c 
        Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c 
        Middlewares/Third_Party/FreeRTOS/Source/tasks.c 
        Middlewares/Third_Party/FreeRTOS/Source/timers.c 
        Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
        Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0/port.c
        Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c

        

        Core/Inc/main.h
        Core/Inc/stm32l0xx_hal_conf.h
        Core/Inc/stm32l0xx_it.h


        ../../src/SCP.c
        Core/Src/main.c
        Core/Src/stm32l0xx_hal_msp.c
        Core/Src/stm32l0xx_it.c
        Core/Src/system_stm32l0xx.c

        startup_stm32l073xx.s)

set(EXECUTABLE ${PROJECT_NAME})



add_executable(${EXECUTABLE} ${STM32CUBEMX_GENERATED_FILES})
set_target_properties(
    ${EXECUTABLE}
    PROPERTIES 
        OUTPUT_NAME ${PROJECT_NAME}
        SUFFIX ".elf"
)
target_compile_definitions(${EXECUTABLE} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32L073xx
        )

target_include_directories(${EXECUTABLE} PRIVATE
        ../../include
        Core/Inc
        Drivers/STM32L0xx_HAL_Driver/Inc
        Drivers/CMSIS/Device/ST/STM32l0xx/Include
        Drivers/CMSIS/Include
        Drivers/CMSIS/Device/ST/STM32L0xx/Include
        Middlewares/Third_Party/FreeRTOS/Source/include
        Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
        Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0
        )

target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m0plus
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=soft

        -fdata-sections
        -ffunction-sections

        -Wall

        $<$<CONFIG:Debug>:-Og>
        )

target_link_options(${EXECUTABLE} PRIVATE
        -T${CMAKE_SOURCE_DIR}/STM32L073RZTx_FLASH.ld
        -mcpu=cortex-m0plus
        -mthumb
#        -mfpu=fpv4-sp-d16
        -mfloat-abi=soft
        -specs=nano.specs
        -specs=nosys.specs
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        )

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE}.elf)

# Create hex file
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE}.elf ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE}.elf ${PROJECT_NAME}.bin)