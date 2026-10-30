# Arduino Optical Communication System

An Arduino Uno project that explores optical message transmission and reception. The sketch frames input values into a fixed-length message, sends the frame using an LED, reads the optical signal with a photoresistor, and uses the received values to drive connected outputs.

**Project status:** Coursework project, demonstrated in the module's in-person assessment in December 2025. The communication loop and the four mini projects ran over the 9-byte message.

Based on the starter sketch "Communications v6" provided by the module (UWE Bristol, Computer Systems Architecture), which supplied the framing, the transmit/receive timing and a button/buzzer example. I adapted it, added the tilt/LED, potentiometer/seven-segment and joystick/fan mini projects, moved the buzzer to pin 11, tuned the receiver threshold to 900, and added a receive-buffer bounds check.

## Overview

The project brings together embedded input handling, a simple framed data format, optical signalling, analogue sensing, and output control. It was developed as part of university coursework on Arduino hardware and communication systems.

## Features

- Frames each message with Start of Text (STX) and End of Text (ETX) markers.
- Collects input from a push button, tilt sensor, potentiometer, and joystick.
- Sends data through a transmitter LED and reads the signal through a photoresistor (analogue receiver).
- Drives a buzzer (button), an indicator LED (tilt), a seven-segment display (potentiometer) and a fan (joystick).
- Reports received character values over the serial interface.

## Technologies

- Platform: Arduino Uno
- Language: Arduino C/C++
- Interfaces: Digital and analogue I/O; serial output
- Signal path: LED optical transmitter and photoresistor receiver

## Message format

Each frame is 9 bytes: one STX marker, seven data fields, and one ETX marker.

| Position | Field | Range |
|---|---|---|
| 1 | Start marker: STX (0x70) | n/a |
| 2 | Button | 0 or 1 |
| 3 | Tilt switch | 0 or 1 |
| 4 | Potentiometer | 0 to 7 |
| 5 | Joystick (field A) | 0 to 127 |
| 6 to 8 | Unused (fields B, C, D) | 0 |
| 9 | End marker: ETX (0x71) | n/a |

The encrypt and decrypt routines are identity functions: they return the data unchanged and do not provide encryption or confidentiality.

## Mini projects

| Mini project | Input | Output |
|---|---|---|
| Button control | Push button (D8) | Buzzer on/off (D11) |
| Tilt switch | Tilt sensor (D2) | Indicator LED on/off (D4) |
| Potentiometer | Potentiometer (A1) | Digit 0 to 7 on a seven-segment display |
| Joystick | Joystick (A4) | Fan on/off at a threshold of 50 (D7) |

## Pin mapping

The table reflects assignments defined in the sketch.

| Component | Arduino Uno pin |
|---|---|
| Optical receiver (photoresistor) | A0 |
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

1. The sketch reads the button, tilt, potentiometer, and joystick inputs.
2. It places the data into a seven-field payload between STX and ETX.
3. The transmitter LED sends the frame bit by bit; the receiver samples the photoresistor and looks for framed data.
4. Received values drive the outputs and are reported through the serial interface.

## Known limitations

- **Marker collision:** The joystick value is mapped to 0 to 127, so readings of 112 and 113 equal the STX (0x70) and ETX (0x71) markers and could be read as frame boundaries. The module brief specifies 0 to 99.
- **No error detection:** The receiver checks the start and stop bits of each character, but there is no checksum, and invalid characters are dropped without any message.
- **Fan output:** The joystick turns the fan on or off at a threshold of 50. D7 is not a hardware PWM pin on a standard Arduino Uno, so variable-speed control would need a PWM-capable pin and updated wiring.
- **Encryption:** The encrypt and decrypt functions leave values unchanged. They are placeholders, not security features.
- **Circuit details:** A wiring diagram and full component specifications are not included.

## Security analysis

The coursework included an 850-word security report on this system. It covered:

- **Interception:** the visible-light channel is unencrypted and unauthenticated, so anyone in line of sight with a photosensor can read it.
- **Manipulation:** a strong external light can jam the receiver, and a rogue LED can spoof messages.
- **Reliability:** there is no checksum, and framing depends only on the start and stop bytes.
- **Hardware:** the photoresistor is slow and low in sensitivity, and physical access allows signals to be injected directly into an input pin.

The countermeasures it proposed were a CRC, Manchester encoding, and shielding the photoresistor or using a faster photodiode.

## Future improvements

- Add a circuit diagram and document component values and receiver thresholds.
- Map the joystick to 0 to 99 so values cannot match the frame markers.
- Add variable-speed fan control on a PWM-capable Uno pin and document the required driver circuit.
- Add a CRC and define how malformed or incomplete frames are handled.
- Try Manchester encoding to make the signal more resistant to ambient light.
- Add automated checks for frame construction and parsing where practical.

## Repository contents

The sketch is named `arduino-optical-communication.ino` to match the repository folder. Open it in the Arduino IDE, select Arduino Uno, and review the pin mapping and circuit requirements before connecting hardware.

## Author

Mostafa Abdelkarim, BSc (Hons) Computer Science, UWE Bristol
