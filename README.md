# Nobs Autopilot — DIY VR-Friendly Autopilot Panel

**Nobs Autopilot is a build-it-yourself control panel for flight simulators** — four knobs and
eight switches that you wire up to a small Arduino board and bind to autopilot controls in MSFS
or DCS. It plugs in over USB and is recognised automatically, with no drivers to install.

This page is the full build guide: what to buy, how to wire it, how to put it together, and how
to set it up in the sim. Basic soldering is part of the build, but you don't need any prior
Arduino experience to follow along.

> **The main board is the Arduino Nano ESP32** — that's what the
> [build instructions](docs/build-instructions.md) and parts list are written for. An older
> **Arduino Micro** build is also supported if that's what you have; it ends up working exactly
> the same, just with different wiring pins and firmware steps:
> - **Arduino Nano ESP32** (main board) → [firmware/arduino_eps32_nano/README.md](firmware/arduino_eps32_nano/README.md)
> - **Arduino Micro** (alternative) → [firmware/arduino_micro/README.md](firmware/arduino_micro/README.md)
>
> If you're using the Micro, use the pin column for it in
> [docs/arduino-micro-wiring.md](docs/arduino-micro-wiring.md) instead of the build instructions' table.

## Virtual Reality (VR) Design Philosophy
Unlike traditional cockpit panels, **this project is engineered specifically for VR flight simulator users**. Because you cannot see your physical hands while wearing a headset, this panel discards all visual screens, LEDs, and digital displays. Instead, it focuses entirely on **tactile memory and physical feedback**:
* **High-Tactile Feedback:** Built using premium mechanical rotary encoders with clear, distinct detents (clicks) and robust subminiature toggle switches that offer a sharp, reassuring snap when thrown.
* **Spatial Layout Optimization:** The generous spacing and deliberate physical grouping of components make it incredibly easy to find, identify, and operate different knobs and buttons purely by touch.


## Project Overview

In short: an Arduino board reads 4 knobs and 8 switches and presents them to your PC as a plain
USB game controller, which the sim binds to autopilot functions. Here's what's inside:

* **The brain (microcontroller):** Arduino Nano ESP32 — *or* an Arduino Micro (the firmware
  supports both).
* **Rotary Encoders (×4):** Bourns PEC11H-4020F-S0016 — the knobs. They click as you turn them and
  can also be pushed in.
* **Momentary Switches (×8):** E-Switch 700SP7B10M2REH — the toggle switches.
* **How the PC sees it:** as a standard USB game controller (HID gamepad), recognised directly by
  MSFS 2024 or DCS — no driver install. It identifies itself as **"Nobs Autopilot"** using a USB ID
  (`303A` / `80F4`) so the Nobs app can pick it out automatically. On the Nano ESP32 build that
  name and ID can be changed on the fly — see
  [docs/board-identity.md](docs/board-identity.md), which also covers running several boxes at once.

> **Note:** Every control wires up the same simple way — one terminal to its assigned Arduino pin,
> the other terminal to common ground. No external resistors are needed; the firmware turns on the
> Arduino's built-in pull-up resistors for you (true for both the Micro and the Nano ESP32).

## Using an Arduino Micro instead of the Nano ESP32

This guide is written around the **Arduino Nano ESP32**, but the project still fully supports the
older **Arduino Micro** — it ends up as the same **"Nobs Autopilot"** device (`303A` / `80F4`) and
behaves identically in the sim, except its name/ID are fixed at compile time instead of
reconfigurable (see [docs/board-identity.md](docs/board-identity.md)). If you go with the Micro,
only **two** things differ:

1. **Wiring pins are different.** Use [docs/arduino-micro-wiring.md](docs/arduino-micro-wiring.md)
   instead of the Nano ESP32 pin table in the [build instructions](docs/build-instructions.md).
2. **Loading the firmware is a little different.** Follow
   [firmware/arduino_micro/README.md](firmware/arduino_micro/README.md) (it needs a one-time
   `boards.txt` edit to set the board's USB name/ID before the first upload).

**Everything else is the same** — the parts list (just swap the board), the 3D-printed enclosure,
the assembly steps, the Nobs app, and the in-sim binding all apply unchanged. So if you're using
the Micro, read the rest of this page normally and just substitute those two things.

## Build Instructions

The hands-on build steps — the full wiring map, the physical pinout reference, the
step-by-step assembly, and first-time verification — live in their own guide:
[docs/build-instructions.md](docs/build-instructions.md). For a visual reference alongside the
wiring map, see the [wiring diagram](docs/wiring-diagram.pdf).

## Bill of Materials (BOM)

The full parts list, core electronics, switches, knobs, fasteners, enclosure parts, and recommended tools, lives in [docs/bill-of-materials.md](docs/bill-of-materials.md).

## Project Costs

Rough cost estimate for a single panel, in USD. These are ballpark figures only, actual prices vary significantly by supplier, region, and quantity, and **exclude shipping and taxes**. Several items (wire, heat-shrink, screws, inserts) are sold in multi-build packs, so the per-build cost is lower if you build more than one or already have them on hand.

| Category | Items | Estimated Cost |
| :--- | :--- | ---: |
| Microcontroller | Arduino Nano ESP32 (genuine; clones are cheaper) | $20 – $28 |
| Rotary encoders | 4 × Bourns PEC11H | $10 – $15 |
| Switches | 8 × E-Switch 700-series momentary | $25 – $35 |
| Board & wiring | Stripboard, pin headers, 8 × terminal blocks | $12 – $18 |
| Knobs & caps | 4 × encoder knobs, 8 × button caps | $15 – $25 |
| Connectors & consumables | USB cable, hook-up wire kit, heat-shrink | $20 – $25 |
| Fasteners | Heat-set inserts, M3/M4 screws | $10 – $18 |
| Enclosure (3D printed) | Filament for top, bottom, mounting plate, front plate | $5 – $15 |
| **Total** | | **≈ $120 – $180** |

> **Notes:**
> * The **enclosure** assumes you print the parts yourself (filament cost only). Ordering them from a print service will add roughly $30 – $80.
> * Choosing an **Arduino Nano ESP32 clone** instead of a genuine board can cut $10 – $15 off the total.
> * Bulk/pack purchasing of consumables and fasteners spreads their cost across multiple builds, lowering the effective per-panel total.

## Firmware

The firmware is the little program that runs on the Arduino and makes it act as the game
controller. There are step-by-step guides for loading it — **follow the one for your board:**

- **Arduino Nano ESP32** (main board): [firmware/arduino_eps32_nano/README.md](firmware/arduino_eps32_nano/README.md)
- **Arduino Micro** (alternative): [firmware/arduino_micro/README.md](firmware/arduino_micro/README.md)

Both give the board the **"Nobs Autopilot"** name and the USB ID (`303A` / `80F4`) the app looks
for, and both expose the same 20 buttons in the same order (each encoder's turn-left, turn-right,
and push, then switches SW1–SW8). The knobs are read with full quadrature decoding so fast turns
count correctly, and each knob's acceleration can be tuned from the app and is saved on the board.

On the Nano ESP32 build the name and ID aren't fixed — they're stored on the board and can be
reassigned (so the same firmware can become any Nobs profile, and you can run several of the same
type). See [docs/board-identity.md](docs/board-identity.md).

*(Under the hood, the Micro build uses the [Arduino Joystick Library](https://github.com/MHeironimus/ArduinoJoystickLibrary),
bundled as [firmware/arduino_micro/ArduinoJoystickLibrary-2.1.1.zip](firmware/arduino_micro/ArduinoJoystickLibrary-2.1.1.zip);
the Nano ESP32 build uses the ESP32's own USB support.)*

## Nobs FS Companion App

The [**Nobs FS app**](https://github.com/ibovegar/nobs-fs-app) is the companion application for communicating with and configuring this panel. It automatically detects the Nobs Autopilot by its USB identity (VID `303A` / PID `80F4`), so the right device is selected even when other game controllers are connected.

Use it to:
* **Verify wiring & test inputs:** watch every encoder rotation, encoder push, and switch register live as you turn or press each control, handy for confirming the build before binding anything in the sim.
* **Configure the encoders:** the Settings page sets each encoder's acceleration sensitivity over the USB serial (CDC) port. The value is persisted to the Arduino's EEPROM, so it survives power cycles and applies in the sim too, no re-flashing required.

See the app repository for installation and usage details: <https://github.com/ibovegar/nobs-fs-app>

## MSFS 2024 Autopilot Control Mapping Template

Not sure what to bind each knob and switch to? This is a suggested layout that covers the common
autopilot controls — Heading, Altitude, Vertical Speed, and Airspeed — plus the usual mode
switches. It's just a starting point; rebind anything to suit how you fly.

### Recommended Button Assignments

| Component Group | Component Label | Virtual HID Button | Action Type | Recommended MSFS 2024 Control Event Name |
| :--- | :--- | :--- | :--- | :--- |
| **Encoder 1 (HDG)** | Encoder 1 | **Button 1** | CW Pulse | **HEADING BUG INCREASE** |
| | Encoder 1 | **Button 2** | CCW Pulse | **HEADING BUG DECREASE** |
| | Encoder 1 | **Button 3** | Push Action | **HEADING SLOT INDEX 1 TOGGLE** *(HDG Sync / Select)* |
| **Encoder 2 (ALT)** | Encoder 2 | **Button 4** | CW Pulse | **AUTOPILOT ALTITUDE SELECTION INCREASE** |
| | Encoder 2 | **Button 5** | CCW Pulse | **AUTOPILOT ALTITUDE SELECTION DECREASE** |
| | Encoder 2 | **Button 6** | Push Action | **AUTOPILOT ALTITUDE LOCK TOGGLE** *(ALT Hold)* |
| **Encoder 3 (V/S)** | Encoder 3 | **Button 7** | CW Pulse | **AUTOPILOT VS INCREASE** |
| | Encoder 3 | **Button 8** | CCW Pulse | **AUTOPILOT VS DECREASE** |
| | Encoder 3 | **Button 9** | Push Action | **AUTOPILOT VS HOLD ON** *(V/S Engage)* |
| **Encoder 4 (SPD)** | Encoder 4 | **Button 10** | CW Pulse | **AUTOPILOT AIRSPEED INCREASE** |
| | Encoder 4 | **Button 11** | CCW Pulse | **AUTOPILOT AIRSPEED DECREASE** |
| | Encoder 4 | **Button 12** | Push Action | **FLIGHT LEVEL CHANGE ON** *(FLC / Speed Hold)* |
| **Switches** | Switch 1 (SW1) | **Button 13** | Push Action | **AUTOPILOT MASTER TOGGLE** *(AP On/Off)* |
| | Switch 2 (SW2) | **Button 14** | Push Action | **TOGGLE FLIGHT DIRECTOR** *(FD Switch)* |
| | Switch 3 (SW3) | **Button 15** | Push Action | **AUTOPILOT APPROACH HOLD TOGGLE** *(APR Mode)* |
| | Switch 4 (SW4) | **Button 16** | Push Action | **AUTOPILOT NAV1 HOLD TOGGLE** *(NAV Mode)* |
| | Switch 5 (SW5) | **Button 17** | Push Action | **AUTO THROTTLE ARM TOGGLE** *(A/THR)* |
| | Switch 6 (SW6) | **Button 18** | Push Action | **TOGGLE BACK COURSE HOLD** *(BC Mode)* |
| | Switch 7 (SW7) | **Button 19** | Push Action | **YAW DAMPER TOGGLE** *(YD On/Off)* |
| | Switch 8 (SW8) | **Button 20** | Push Action | **AUTOPILOT PANEL LIGHTS TOGGLE** *(Backlight Control)* |

### In-Game Configuration Instructions

1. Launch **Microsoft Flight Simulator 2024** and navigate to **Options > Control Options**.
2. Locate your uniquely named device (**"Nobs Autopilot"**) from the controller list at the top.
3. Switch the filter layout on the left column from `ESSENTIAL` or `ASSIGNED` to **`ALL`** to expose all hidden input categories.
4. Expand the **Autopilot** category sub-menu.
5. Click on the command field you wish to bind (e.g. *Heading Bug Increase*), select **Start Scanning**, and turn or press the corresponding physical component on your panel. The simulator will auto-detect the virtual HID button number.
6. Click **Validate** and save your profile configuration.
</content>
</invoke>
