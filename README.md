### Introduction
[Pi-KVM](http://pikvm.org "Pi-KVM") is a Raspberry Pi based open source KVM implementation.  It  can support USB keyboard and mouse emulation through USB HID.  It currently uses AVR-based HID implementation.  However, I only have STM32F103 [BluePill](https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill.html "BluePill") board available.  So I decided to do a simlar implementation using cheap BluePill.   This repo contains the HID emulation source code for my implementation on STM32F103.  It has been tested on Pi-KVM, and it worked well for me.

### Build Instrucitons
Comping Pi-KVM HID emulator has been tested with Ubuntu 20.04 distribution.  Please make sure you have arm-non-eabi-gcc toolchain installed.
To compile:
```c
    make clean
    make
```
The final generated image will be located at  ./out/pikvm_hid.bin.  This image needs to be flashed to STM32F103 flash address 0x8000000.

### Program BluePill
- Enable boot mode to boot from ROM
	-	Set jumper BOOT0 to position 1 (3.3v)
- Connect USB UART adaptor to Blue Pill UART1
	- Host TXD to STM32F103 RXD (PA10)
	- Host RXD to STM32F103 TXD (PA9)
	- Host GND to STM32F103 GND
- Flash binary to BluePill.   Run following command:
	- sudo python ./script/stmloader.py -p /dev/ttyUSB0 -f F1 -e -w -v ./out/pikvm_hid.bin
- Enable boot mode to normal mode
	-	Set jumper BOOT0 to position 0 (GND)

### Connect to PiPVM
Pi-KVM communicate with HID emulator through primary UART. It is GPIO 14 (TX, Pin 8) and GPIO 15 (RX, Pin 10) on Raspberry Pi 40-pin connector.  For BluePill HID emulator, it uses STM32F103 UART3 (TX PB10 and RX PB11) for Pi-KVM communication.  So they need to be connected as below:
-	RPI TX (Pin 8)     <=>  BluePill RX PB11
-	RPI RX (Pin 10)   <=>  BluePill TX PB10
-	RPI GND (Pin 6)  <=>  BluePill GND

### References
This project reused some souce code from:
- https://github.com/STMicroelectronics/STM32CubeF1
- https://github.com/IntergatedCircuits/USBDevice
- https://github.com/navilera/Navilos
