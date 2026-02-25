# Wiring Overview

## Microcontroller
Arduino Pro Micro (ATmega32U4)

## LCD (16x2 I2C)

- Address: 0x27
- VCC → 5V
- GND → GND
- SDA → SDA
- SCL → SCL

## Keypad Matrix

### Row Pins
- R1 → D8
- R2 → D9
- R3 → D10
- R4 → A2

### Column Pins
- C1 → D4
- C2 → D5
- C3 → D6
- C4 → D7
- C5 → A1

## USB

USB connection is used for:
- Power
- HID keyboard emulation
