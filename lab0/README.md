# Intro to STM32 Nucleo Board: Blink LED

This lab introduces the STM32 Nucleo development platform and the embedded toolchain:
- **STM32 Nucleo-F446ZE Board:** ARM-Cortex M4 CPU
- **Keil Microcontroller Development Kit (MDK):** using Keil µVision Integrated Development Environment (IDE)
- **Hercules:** serial terminal program to communicate with board over a COM port (serial/USB) connection

## Blink-y Project Summary:
- Project toggles the on-board LEDs using GPIO control
- Used HAL (Hardware Abstraction Layer) initialization functions (HAL_Init, SystemClock_Config, MX_GPIO_Init) to configure peripherals
- Implemented UART serial communication to print messages from the board to the PC terminal (through Hercules)

![Nucleo User LEDs blinking](https://github.com/pietrea2/STM32-Embedded-Projects/blob/main/lab0/images/leds.jpg)

![Printing to COM using Hercules](https://github.com/pietrea2/STM32-Embedded-Projects/blob/main/lab0/images/hercules_uart_printf.png)