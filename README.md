# Hackpad

Hackpad is a hardware-assisted penetration testing input device designed to accelerate reconnaissance workflows.  
It combines a physical keypad interface with USB HID keyboard emulation to launch predefined security tools directly from a target system.

⚠ This tool is intended strictly for authorized security testing and educational purposes.

---

## Features

- IPv4 and TCP port input via physical keypad
- Dual tool banks (IP tools / Web tools)
- Integrated 16x2 I2C LCD interface
- USB HID keyboard emulation (no drivers required)
- Norwegian keyboard layout support
- Input validation (IPv4 + port range checking)
- Modular command execution per selected tool

---

## Supported Tools

### IP Tool Bank
- Nmap
- Nikto
- Gobuster

### Web Tool Bank
- sqlmap
- WhatWeb
- theHarvester

---

## Hardware Overview

- Arduino Pro Micro (ATmega32U4)
- 4x5 matrix keypad
- 16x2 I2C LCD display (0x27)
- USB connection to host machine

---

## How It Works

1. User enters target IPv4 address.
2. Optional TCP port can be specified.
3. One or more tools are selected.
4. Hackpad opens a terminal (Hyprland Super+Enter).
5. The selected commands are typed automatically via USB HID.

No software is installed on the target system — the device acts as a keyboard.

---

## Building / Flashing

### Required Libraries
- Keypad
- LiquidCrystal_I2C
- HID-Project (NicoHood)

Board selection:
- Arduino Leonardo / Pro Micro (ATmega32U4)

Upload via Arduino IDE.

---

## Project Structure
firmware/esp32/ → Main firmware (.ino)
hardware/ → PCB and enclosure files
docs/ → Documentation


## Roadmap

- OLED display support
- Command profile presets
- On-device logging
- Wireless version (ESP32)
- Modular plugin architecture

---

## Disclaimer

This project is provided for educational and authorized penetration testing use only.  
The author assumes no responsibility for misuse.
