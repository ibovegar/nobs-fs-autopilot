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
- Runtime-configurable USB identity for the Arduino Nano ESP32 build: PID and product name are stored in flash (NVS) and can be reassigned over USB serial with `SET_ID`/`GET_ID`, so each physical panel can be told apart by MSFS. Documented in [docs/board-identity.md](docs/board-identity.md).
- Arduino Micro pinout reference ([docs/arduino-micro-wiring.md](docs/arduino-micro-wiring.md)), split out now that the wiring docs are written around the Nano ESP32.
- Power/status LED on the Nano ESP32 build, wired to **D4** via a 120 Ω resistor: blinks while booting/waiting for USB enumeration, lights steady once the host PC has enumerated it.

### Changed
- The **Arduino Nano ESP32** is now the project's main board; the build instructions, BOM, and wiring docs are written around it, with the Arduino Micro documented as a supported alternative.
- Both firmware variants now report VID `0x303A` (Espressif's vendor ID) instead of `0x2341`, alongside PID `0x80F4` / "Nobs Autopilot".
- Corrected the BOM's USB cable spec to USB-C for the Nano ESP32 (was Micro-B, which only applies to the Arduino Micro alternative).
- Renamed `docs/Wiring diagram.pdf` to `docs/wiring-diagram.pdf` and linked it from the wiring and build-instructions docs.
- Split `docs/wiring-diagram.pdf` into separate per-board diagrams, `docs/wiring-diagram-esp32-nano.pdf` and `docs/wiring-diagram-micro.pdf`, and updated the wiring/build-instructions links to match.
- Moved the build instructions out of the README into their own guide ([docs/build-instructions.md](docs/build-instructions.md)), and added photos of the breakout board and enclosure integration steps ([images/](images/)).
- Restructured README.md to match the leaner style used by the other Nobs panel projects: a short intro, the VR design philosophy, a "Docs" link list, and a brief device-ID section, with the detailed wiring/firmware/cost content left to its own doc page.

### Removed
- The MSFS 2024 button-mapping template from the README (no replacement; it wasn't being kept up to date with the in-sim binding workflow).

[Unreleased]: https://github.com/ibovegar/nobs-fs-autopilot/commits/main
