# Nobs Autopilot — DIY VR-Friendly Autopilot Panel

A build-it-yourself control panel for flight simulators — **4 rotary encoders** and **8 toggle
switches** wired to an **Arduino Nano ESP32** (an older **Arduino Micro** is also supported). It
plugs in over USB and shows up as a standard game controller (HID gamepad) named **"Nobs
Autopilot"**, recognised directly by MSFS or DCS and the Nobs app, with no drivers to install.

Each encoder's turn-left, turn-right, and push are reported as 3 buttons (12 total); the 8 toggle
switches add 8 more, for **20 buttons** total.

## Virtual Reality (VR) Design Philosophy

Unlike traditional cockpit panels, **this project is engineered specifically for VR flight
simulator users**. Because you cannot see your physical hands while wearing a headset, this panel
discards all visual screens, LEDs, and digital displays. Instead, it focuses entirely on **tactile
memory and physical feedback**:
* **High-Tactile Feedback:** Built using premium mechanical rotary encoders with clear, distinct
  detents (clicks) and robust subminiature toggle switches that offer a sharp, reassuring snap when
  thrown.
* **Spatial Layout Optimization:** The generous spacing and deliberate physical grouping of
  components make it incredibly easy to find, identify, and operate different knobs and buttons
  purely by touch.

## Docs

- **[Build instructions](docs/build-instructions.md)** — wiring map, physical pinout reference,
  step-by-step assembly with photos, and first-time verification.
- **[Which wire goes where](docs/arduino-esp-32-wiring.md)** — the button + pin map for the Nano
  ESP32 build.
- **[Loading the firmware](firmware/arduino_eps32_nano/README.md)** — first-time flashing and
  re-flashing, step by step.
- **[Setting the device ID & name](docs/board-identity.md)** — how the board names itself, how to
  rename it, and how to run several boxes at once.
- **[Bill of materials](docs/bill-of-materials.md)** — parts list and project cost estimate.

## Using an Arduino Micro instead of the Nano ESP32

This guide is written around the **Arduino Nano ESP32**, but the project still fully supports the
older **Arduino Micro** — it ends up as the same **"Nobs Autopilot"** device (`303A` / `80F4`) and
behaves identically in the sim, except its name/ID are fixed at compile time instead of
reconfigurable (see [docs/board-identity.md](docs/board-identity.md)). If you go with the Micro,
two things differ:

1. **Wiring pins** — use [docs/arduino-micro-wiring.md](docs/arduino-micro-wiring.md) instead of
   the Nano ESP32 pin table in the [build instructions](docs/build-instructions.md).
2. **Loading the firmware** — follow
   [firmware/arduino_micro/README.md](firmware/arduino_micro/README.md) instead (it needs a
   one-time `boards.txt` edit to set the board's USB name/ID before the first upload).

Everything else — the parts list (just swap the board), the 3D-printed enclosure, the assembly
steps, the Nobs app, and the in-sim binding — is unchanged.

## Nobs FS Companion App

The [**Nobs FS app**](https://github.com/ibovegar/nobs-fs-app) is the companion application for
communicating with and configuring this panel. It automatically detects the Nobs Autopilot by its
USB identity (VID `303A` / PID `80F4`), so the right device is selected even when other game
controllers are connected.

Use it to:
* **Verify wiring & test inputs:** watch every encoder rotation, encoder push, and switch register
  live as you turn or press each control, handy for confirming the build before binding anything in
  the sim.
* **Configure the encoders:** the Settings page sets each encoder's acceleration sensitivity over
  the USB serial (CDC) port. The value is persisted to the Arduino's EEPROM, so it survives power
  cycles and applies in the sim too, no re-flashing required.

See the app repository for installation and usage details: <https://github.com/ibovegar/nobs-fs-app>

## Setting the device ID & name (in brief)

The board's name and USB product ID aren't compiled in — they're stored on the board (Nano ESP32
build only), so the same firmware can be set up as any Nobs profile. Out of the box this is
**"Nobs Autopilot"** (`303A` / `80F4`). To change it, the configuration app sends a single line
over the board's serial port:

```
SET_ID:80F4:Nobs Autopilot
```

The board saves the new name + ID, replies `OK:80F4:Nobs Autopilot`, and reboots so it takes effect
(`GET_ID` reads back the current values). For multiple autopilots, give each one the next ID in the
block — e.g. `SET_ID:80F5:Nobs Autopilot 2`. Full details, including the Windows name-cache
refresh, are in **[docs/board-identity.md](docs/board-identity.md)**.
