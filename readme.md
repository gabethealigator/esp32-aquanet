
# ESP32 AquaNet

ESP32 AquaNet is a project aimed at creating a networked system for monitoring and controlling aquatic environments using an ESP32 microcontroller. This project is developed as part of the final project (TCC) for the Systems Development course.

## Table of Contents

- [Introduction](#introduction)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Configuration](#configuration)
- [Code Structure](#code-structure)
- [Code Explanation](#code-explanation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Introduction

The ESP32 AquaNet project utilizes the ESP32 microcontroller to monitor and control various aspects of an aquatic environment. The system is designed to be highly configurable and extendable, allowing users to tailor it to their specific needs.

## Hardware Requirements

- ESP32 Development Board
- Sensors (e.g., temperature, pH, turbidity)
- Actuators (e.g., pumps, lights)
- Power supply
- Breadboard and jumper wires

## Software Requirements

- Arduino IDE or PlatformIO
- ESP32 board support in Arduino IDE/PlatformIO
- Required libraries (listed in `platformio.ini` or `lib` directory)

## Installation

### Arduino IDE

1. Install the Arduino IDE.
2. Add ESP32 board support through the Board Manager.
3. Clone this repository and open the project in Arduino IDE.
4. Install the required libraries listed in `platformio.ini` or the `lib` directory.

### PlatformIO

1. Install Visual Studio Code.
2. Install the PlatformIO extension.
3. Clone this repository and open it in PlatformIO.
4. PlatformIO will automatically install the required libraries.

## Configuration

Before uploading the code to your ESP32, configure the following parameters:

1. WiFi credentials
2. Sensor and actuator pins
3. Other customizable settings as needed

## Code Structure

The project is organized into the following directories:

- `include`: Header files for the project
- `lib`: External libraries used in the project
- `src`: Source code files
- `test`: Test files for the project

Key files:

- `src/main.cpp`: The main entry point for the project
- `platformio.ini`: PlatformIO configuration file

## Code Explanation

The main code file `src/main.cpp` handles the setup and loop functions essential for the ESP32 operation.

### LED Indications

- **Initial Setup Prompt**: The LED blinks once every second for 3 seconds, prompting the user to press the setup button if they want to enter setup mode.
- **Entering Setup Mode**: If the setup button is pressed, the LED blinks twice rapidly to indicate that the setup mode has been activated.
- **WiFi Connection Status**: Once the device attempts to connect to WiFi, if successful, the LED blinks every 4 seconds to indicate a successful connection.

### Custom Parameter Classes Examples

Three custom parameter classes are defined to handle different types of parameters:

#### IPAddressParameter

Handles IP address parameters.

#### IntParameter

Handles integer parameters.

#### FloatParameter

Handles float parameters.

### Settings Structure

A structure to hold various configuration settings:

```cpp
struct Settings
{
  float f;
  int i;
  char s[20];
  uint32_t ip;
} sett;
```

### Setup Function

The `setup` function initializes the WiFi mode, sets up pin modes, and reads saved settings from EEPROM. It checks if the setup button is pressed to enter the configuration portal where WiFi credentials and custom parameters can be set.

### Loop Function

The `loop` function continuously checks the WiFi connection status and blinks the LED based on the connection status.

## Usage

1. Connect the hardware components as described in the Hardware Requirements section.
2. Configure the software as described in the Configuration section.
3. Upload the code to the ESP32.
4. Monitor and control your aquatic environment using the ESP32.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your improvements.

## License

This project is licensed under the MIT License. See the LICENSE file for details.
