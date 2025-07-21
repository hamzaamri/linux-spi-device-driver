# ADXL345 SPI Linux Kernel Driver

This is a basic Linux kernel module for interfacing with the ADXL345 accelerometer sensor via SPI. It registers a character device and allows userspace to read acceleration data directly from the sensor.

## Features

- SPI driver for ADXL345 accelerometer
- Registers a character device at `/dev/adxl345`
- Supports read operation using SPI transactions
- Device tree compatible (`adi,adxl345`)

## Requirements

- Linux kernel headers installed (e.g. `/usr/src/linux-headers-$(uname -r)`)
- SPI-enabled hardware (e.g. Raspberry Pi, Jetson Nano, or other ARM boards)
- Kernel version 5.x or newer

## Build

```bash
    make
