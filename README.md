# Arduino Optical Communication System

An Arduino Uno project that explores optical message transmission and reception. The sketch frames input values into a fixed-length message, sends the frame using an LED, reads the optical signal with an analogue sensor, and uses the received values to drive connected outputs.

> **Project status:** The repository contains the Arduino sketch. End-to-end operation with the physical circuit has not been verified.

## Overview

The project brings together embedded input handling, a simple framed data format, optical signalling, analogue sensing, and output control. It was developed as part of university coursework on Arduino hardware and communication systems.

## Features

- Frames each message with Start of Text (`STX`) and End of Text (`ETX`) markers.
- Collects input from a push button, tilt sensor, potentiometer, and joystick.
- Sends data through a transmitter LED and reads the signal through an analogue receiver sensor.
- Includes output handling for a buzzer, indicator LED, fan output, and seven-segment display.
- Reports received character values over the serial interface.

## Technologies

- **Platform:** Arduino Uno
- **Language:** Arduino C/C++
- **Interfaces:** Digital and analogue I/O; serial output
- **Signal path:** LED optical transmitter and analogue sensor receiver

## Message format

Each frame is **9 bytes**: one `STX` marker, seven data fields, and one `ETX` marker.

| Position | Contents |
|---:|---|
| 1 | Start marker: `STX` |
| 2–8 | Seven data fields |
| 9 | End marker: `ETX` |

The markers identify the message boundaries; the seven middle positions carry the project’s input values. The `encrypt` and `decrypt` routines are identity functions: they return the data unchanged and do **not** provide encryption or confidentiality.

## Pin mapping

The table reflects assignments defined in the sketch. It documents the code configuration and is not a verified circuit diagram.

| Component | Arduino Uno pin |
|---|---|
| Optical receiver sensor | A0 |
| Potentiometer | A1 |
| Joystick analogue input | A4 |
| Push button | D8 |
| Tilt sensor | D2 |
| Transmitter LED | D3 |
| Tilt indicator LED | D4 |
| Fan output | D7 |
| Buzzer | D11 |
| Seven-segment display | D5, D6, D9, D10, D12, D13, A2 |

## How it works

1. The sketch reads the configured button, tilt, potentiometer, and joystick inputs.
2. It places the data into a seven-field payload between `STX` and `ETX`.
3. The transmitter LED represents the outgoing signal; the receiver logic samples the analogue sensor and looks for framed data.
4. Received values are used by the output logic and reported through the serial interface.

This describes the software flow. The physical transmitter/receiver arrangement and end-to-end behaviour have not been independently validated.

## Known limitations and testing status

- **Hardware testing:** End-to-end operation with an Arduino and optical circuit is not currently verified.
- **Fan output:** The sketch uses D7 for on/off control. D7 is not a hardware PWM pin on a standard Arduino Uno, so variable-speed control requires moving the fan control to a PWM-capable pin and updating the wiring.
- **Encryption:** The `encrypt` and `decrypt` functions leave values unchanged; they should be treated as placeholders, not security features.
- **Circuit details:** The source pin assignments are available, but a verified wiring diagram and complete component specifications are not included here.

## Future improvements

- Verify the wiring and test transmission and reception on the target hardware.
- Add a circuit diagram and document component values and receiver thresholds.
- Add variable-speed fan control on a PWM-capable Uno pin and document the required driver circuit.
- Add error detection, such as a checksum, and define how malformed or incomplete frames are handled.
- Document the seven payload fields and their value ranges directly alongside the frame definition.
- Add automated checks for frame construction and parsing where practical.

## Repository contents

The sketch is named `arduino-optical-communication.ino` to match the repository folder. Open it in the Arduino IDE, select **Arduino Uno**, and review the pin mapping and circuit requirements before connecting hardware.
