# Hackpad

Hackpad is a hardware-assisted penetration testing input device designed to accelerate reconnaissance workflows.  
It combines a physical keypad interface with USB HID keyboard emulation to launch predefined security tools directly from a target system.

⚠ This tool is intended strictly for authorized security testing and educational purposes.

---

## Photos

![Hackpad Top](images/Hackpad-top.jpg)

**More angles:**
![Hackpad Side](images/Hackpad-side.jpg)
![Hackpad Desk](images/Hackpad-desk.jpg)
![Hackpad Buttons](images/Hackpad-buttons.jpg)

## How It Works

1. User enters target IPv4 address.
2. Optional TCP port can be specified.
3. One or more tools are selected.
4. Hackpad opens a terminal (Hyprland Super+Enter).
5. The selected commands are typed automatically via USB HID.

No software is installed on the target system — the device acts as a keyboard.

---


## Physical Key Layout

The keypad is physically labeled for intuitive operation (Mode, Clear, Port, Backspace, Enter, Opt1–Opt3, Hack).  
Internally, the firmware maps these labels to key events as follows:

| Button label | Firmware key | Function |
|---|---:|---|
| Mode | `M` | Switch between IP and Web tool banks |
| Clear | `C` | Clear current input (IP/port) |
| Port | `P` | Toggle between IP and Port edit mode |
| Backspace | `B` | Delete last character |
| Enter | `E` | Validate + save current input |
| Opt1 | `n` | Toggle tool 1 (Nmap / sqlmap) |
| Opt2 | `i` | Toggle tool 2 (Nikto / WhatWeb) |
| Opt3 | `g` | Toggle tool 3 (Gobuster / theHarvester) |
| Hack | `h` | Execute selected tools |


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

## Physical Interface

![Hackpad Top](images/Hackpad-buttons.jpg)

The keypad is physically labeled for intuitive operation:

| Button label | Firmware key | Function |
|---|---:|---|
| Mode | M | Switch between IP and Web tool banks |
| Clear | C | Clear current input (IP/port) |
| Port | P | Toggle between IP and Port edit mode |
| Backspace | B | Delete last character |
| Enter | E | Save current input |
| Opt1 | n | Toggle tool 1 (Nmap / sqlmap) |
| Opt2 | i | Toggle tool 2 (Nikto / WhatWeb) |
| Opt3 | g | Toggle tool 3 (Gobuster / theHarvester) |
| Hack | h | Execute selected tools |


## Building / Flashing

### Required Libraries
- Keypad
- LiquidCrystal_I2C
- HID-Project (NicoHood)

Board selection:
- Arduino Leonardo / Pro Micro (ATmega32U4)

Upload via Arduino IDE.

---

# Hardware 

## Microcontroller
- Arduino Pro Micro (ATmega32U4, 5V / 16MHz)

## Display
- 16x2 LCD Display
- I2C Backpack (PCF8574, address 0x27)

## Input System
- 20x Mechanical switches (4x5 matrix)
- 20x 1N4148 diodes
- Handwired matrix

## Enclosure
- 3D printed case (PLA)
- 3D printed keycaps with engraved labels

## Additional
- USB cable (Micro-USB)
- Jumper wires

## 3D Enclosure

![Hackpad Stl](images/Hackpad-stl.jpg)

The 3D printable enclosure files are available in:

hardware/enclosure/

The design includes:
- Main case
- Top plate
- Keycap set

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
