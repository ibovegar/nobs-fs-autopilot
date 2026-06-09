# Changelog

All notable changes to the Nobs Autopilot project (hardware, firmware, and documentation) are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Initial project documentation in [README.md](README.md): VR design philosophy, hardware connection scheme, physical pinout reference, project cost estimate, and step-by-step assembly instructions.
- Arduino Micro firmware sketch ([firmware/arduino_micro/arduino_micro.ino](firmware/arduino_micro/arduino_micro.ino)) exposing 20 USB HID buttons (4 encoders × CW/CCW/push, plus 8 momentary switches) with full quadrature decoding, pulse-paced rotation steps, and host-configurable per-encoder acceleration over USB serial.
- Custom USB identity for the panel: VID `0x2341`, PID `0x0657`, product name "Nobs Autopilot".
- Firmware flashing and USB identity guide ([firmware/arduino_micro/README.md](firmware/arduino_micro/README.md)).
- Bill of Materials ([docs/bill-of-materials.md](docs/bill-of-materials.md)) and enclosure dimensions ([docs/dimensions.pdf](docs/dimensions.pdf)).
- 3D-printable enclosure models: top, bottom, front plate, and mounting plate ([models/](models/)).
- Wiring diagram and component library ([schematics/](schematics/)).
- MSFS 2024 autopilot control mapping template covering Heading, Altitude, Vertical Speed, and Airspeed.
- Bundled [Arduino Joystick Library](https://github.com/MHeironimus/ArduinoJoystickLibrary) (v2.1.1) used by the firmware.
- Placeholder for an ESP32 Nano firmware variant ([firmware/arduino_eps32_nano/](firmware/arduino_eps32_nano/)).

[Unreleased]: https://github.com/ibovegar/nobs-fs-autopilot/commits/main
