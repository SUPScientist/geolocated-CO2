# Getting Started
This repository includes firmware and data analysis code for a CO2 sensor (the SenseAir K-30, supplied by CO2Meter.com) with integrated datalogging and GPS time/location services.

## Components
- Adafruit M0 Adalogger (an Adafruit Feather microcontroller board with an ARM M0 processor and integrated SD logging)
- SenseAir K-30 (an NDIR CO2 sensor)
- Adafruit GPS FeatherWing (an Adafruit FeatherWing add-on for the Feather microcontroller with GPS)
- Rechargeable LiPo battery, 3.7 V nominal

## Firmware
The current best firmware to operate the device is in the subdirectory k-30_read-and-save_SERCOMSerial_GPS. Multiple peripherals use UART serial communications and the Adalogger typically provides access to only one hardware UART, so the SERCOM solution enables use of a second. 

## Data Analysis
The Analysis subdirectory provides example code (Python) to visualize the geolocated CO2 data.