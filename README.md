# Nobs Autopilot — DIY VR-Friendly Autopilot Panel

**Nobs Autopilot is a build-it-yourself control panel for flight simulators** — four knobs and
eight switches that you wire up to a small Arduino board and bind to autopilot controls in MSFS
or DCS. It plugs in over USB and is recognised automatically, with no drivers to install.

This page is the full build guide: what to buy, how to wire it, how to put it together, and how
to set it up in the sim. Basic soldering is part of the build, but you don't need any prior
Arduino experience to follow along.

> **Two board choices — pick whichever you have.** Both end up working exactly the same; they
> just differ in wiring pins and how you load the firmware:
> - **Arduino Micro** → [firmware/arduino_micro/README.md](firmware/arduino_micro/README.md)
> - **Arduino Nano ESP32** → [firmware/arduino_eps32_nano/README.md](firmware/arduino_eps32_nano/README.md)
>
> The wiring table and parts list below show the **Arduino Micro** build. If you're using the
> Nano ESP32, use the pin column for it in [docs/arduino-esp-32-wiring.md](docs/arduino-esp-32-wiring.md).

## Virtual Reality (VR) Design Philosophy
Unlike traditional cockpit panels, **this project is engineered specifically for VR flight simulator users**. Because you cannot see your physical hands while wearing a headset, this panel discards all visual screens, LEDs, and digital displays. Instead, it focuses entirely on **tactile memory and physical feedback**:
* **High-Tactile Feedback:** Built using premium mechanical rotary encoders with clear, distinct detents (clicks) and robust subminiature toggle switches that offer a sharp, reassuring snap when thrown.
* **Spatial Layout Optimization:** The generous spacing and deliberate physical grouping of components make it incredibly easy to find, identify, and operate different knobs and buttons purely by touch.


## Project Overview

In short: an Arduino board reads 4 knobs and 8 switches and presents them to your PC as a plain
USB game controller, which the sim binds to autopilot functions. Here's what's inside:

* **The brain (microcontroller):** Arduino Micro — *or* an Arduino Nano ESP32 (the firmware
  supports both).
* **Rotary Encoders (×4):** Bourns PEC11H-4020F-S0016 — the knobs. They click as you turn them and
  can also be pushed in.
* **Momentary Switches (×8):** E-Switch 700SP7B10M2REH — the toggle switches.
* **How the PC sees it:** as a standard USB game controller (HID gamepad), recognised directly by
  MSFS 2024 or DCS — no driver install. It identifies itself as **"Nobs Autopilot"** using a
  custom USB ID (`2341` / `0657`) so the Nobs app can pick it out automatically.

> **Note:** Every control wires up the same simple way — one terminal to its assigned Arduino pin,
> the other terminal to common ground. No external resistors are needed; the firmware turns on the
> Arduino's built-in pull-up resistors for you (true for both the Micro and the Nano ESP32).

## Using an Arduino Nano ESP32 instead of the Micro

This guide is written around the **Arduino Micro**, but the project fully supports the **Arduino
Nano ESP32** too — it ends up as the exact same **"Nobs Autopilot"** device (`2341` / `0657`) and
behaves identically in the sim. If you go with the Nano ESP32, only **two** things differ:

1. **Wiring pins are different.** Use the *"Arduino Nano ESP32"* column in
   [docs/arduino-esp-32-wiring.md](docs/arduino-esp-32-wiring.md) instead of the Micro pin table
   below.
2. **Loading the firmware is a little different.** Follow
   [firmware/arduino_eps32_nano/README.md](firmware/arduino_eps32_nano/README.md) (it uses a small
   `build_opt.h` file and a double-tap-RESET step when re-uploading).

**Everything else is the same** — the parts list (just swap the board), the 3D-printed enclosure,
the assembly steps, the Nobs app, and the in-sim binding all apply unchanged. So if you're using
the Nano ESP32, read the rest of this page normally and just substitute those two things.

## Hardware Connection Scheme

This is the wiring map: each control has one wire to a numbered pin on the Arduino, and one wire
to **GND** (ground). That's all — you don't need any resistors or extra parts, because the
firmware takes care of that for you.

The table below is for the **Arduino Micro**. Using an **Arduino Nano ESP32**? The pins are
different — see [docs/arduino-esp-32-wiring.md](docs/arduino-esp-32-wiring.md) for its column.

| Component Group | Component Label | Hardware Connection | Arduino Micro Pin | Virtual HID Button | Action Type |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Encoder 1** | Encoder 1 | Phase A Terminal | **A0** | Button 1 | CW Pulse |
| | Encoder 1 | Phase B Terminal | **A1** | Button 2 | CCW Pulse |
| | Encoder 1 | Switch Terminal 1 | **A2** | **Button 3** | Push |
| **Encoder 2** | Encoder 2 | Phase A Terminal | **A3** | Button 4 | CW Pulse |
| | Encoder 2 | Phase B Terminal | **A4** | Button 5 | CCW Pulse |
| | Encoder 2 | Switch Terminal 1 | **A5** | **Button 6** | Push |
| **Encoder 3** | Encoder 3 | Phase A Terminal | **D3** | Button 7 | CW Pulse |
| | Encoder 3 | Phase B Terminal | **D2** | Button 8 | CCW Pulse |
| | Encoder 3 | Switch Terminal 1 | **D4** | **Button 9** | Push |
| **Encoder 4** | Encoder 4 | Phase A Terminal | **D0 (RX)** | Button 10 | CW Pulse |
| | Encoder 4 | Phase B Terminal | **D1 (TX)** | Button 11 | CCW Pulse |
| | Encoder 4 | Switch Terminal 1 | **D16 (MOSI)** | **Button 12** | Push |
| **Switches** | Switch 1 (SW1) | Terminal 2 (N.O.) | **D12** | **Button 13** | Push |
| | Switch 2 (SW2) | Terminal 2 (N.O.) | **D11** | **Button 14** | Push |
| | Switch 3 (SW3) | Terminal 2 (N.O.) | **D10** | **Button 15** | Push |
| | Switch 4 (SW4) | Terminal 2 (N.O.) | **D9** | **Button 16** | Push |
| | Switch 5 (SW5) | Terminal 2 (N.O.) | **D8** | **Button 17** | Push |
| | Switch 6 (SW6) | Terminal 2 (N.O.) | **D7** | **Button 18** | Push |
| | Switch 7 (SW7) | Terminal 2 (N.O.) | **D6** | **Button 19** | Push |
| | Switch 8 (SW8) | Terminal 2 (N.O.) | **D5** | **Button 20** | Push |
| **Ground Loop** | All Components | Common / GND Terminals | **GND** | *None* | System Ground Reference |

> **Pin notes:**
> * Encoder 4's push switch sits on **D16 (MOSI)** rather than the adjacent D17 — D17 drives the on-board RX LED, which holds the line low and would register a phantom press.
> * Encoder 4's Phase A/B share the **D0/D1** UART pins. These stay free for digital I/O because the Arduino Micro talks to the host over native USB CDC, not the hardware UART.

## Physical Pinout Reference

### Rotary Encoder Layout
Viewed from the bottom of the housing with the three closely spaced pins facing downward:

```text
       [ Top: Encoder Shaft Side ]

         (A)      (C/GND)     (B)     <-- 3 Pins (Rotation Control)

          |          |         |

        (SW1)                (SW2)    <-- 2 Pins (Push Button Switch)
```
* **Wiring Rotation:** Connect `A` and `B` to their designated Arduino pins. Connect the middle pin `C/GND` to your master ground loop.
* **Wiring Button:** Connect `SW1` to its designated Arduino push-button pin. Connect `SW2` directly to your master ground loop.

### Momentary Switch Toggle Layout
Viewed from the rear showing three vertically aligned gold terminals:

```text
         [ ]  Terminal 1 (Common - C)             --> Connect to Master Ground Loop
         [ ]  Terminal 2 (Normally Open - N.O.)   --> Connect to Arduino Pin
         [ ]  Terminal 3 (Normally Closed - N.C.) --> LEAVE DISCONNECTED
```

## Bill of Materials (BOM)

The full parts list, core electronics, switches, knobs, fasteners, enclosure parts, and recommended tools, lives in [docs/bill-of-materials.md](docs/bill-of-materials.md).

## Project Costs

Rough cost estimate for a single panel, in USD. These are ballpark figures only, actual prices vary significantly by supplier, region, and quantity, and **exclude shipping and taxes**. Several items (wire, heat-shrink, screws, inserts) are sold in multi-build packs, so the per-build cost is lower if you build more than one or already have them on hand.

| Category | Items | Estimated Cost |
| :--- | :--- | ---: |
| Microcontroller | Arduino Micro (genuine; clones are cheaper) | $20 – $28 |
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
> * Choosing an **Arduino Micro clone** instead of a genuine board can cut $10 – $15 off the total.
> * Bulk/pack purchasing of consumables and fasteners spreads their cost across multiple builds, lowering the effective per-panel total.

## Assembly Instructions

Here's how the panel goes together, in order. There's soldering involved, so take your time and
work in a well-lit, ventilated spot. Don't rush — it's much easier to get each joint right the
first time than to fix it later. If a term is unfamiliar, a quick search will usually show you
exactly what it looks like.

The build happens in four stages: (1) prep the little circuit board the Arduino sits on, (2) wire
up the switches and knobs, (3) mount everything to the front plate, and (4) close it all up in
the case.

### Phase 1: Breakout Board Assembly
* **Fit Headers:** Position female socket headers onto the stripboard to match the Arduino Micro profile and solder them from underneath.
* **Mount Terminal Blocks:** Place the screw terminal blocks flanking the socket headers on the stripboard and solder them into place.
* **Bridge Traces:** Ensure the copper traces on the stripboard securely connect each header pin footprint directly to its corresponding terminal block clamping slot.
* **Isolate Rails:** Use a track cutter or utility knife to score and break any copper traces running between opposing header pin rows to prevent short circuits.

### Phase 2: Component Pre-Wiring
* **Prep Signal Lines:** Cut hook-up wire into 15–20 cm lengths. Strip 3 mm of insulation off both ends of each wire run.
* **Wire the Switches:** Slide heat shrink tubing over your wires. Solder a signal wire to the N.O. terminal and a ground wire to the common terminal of each switch. Leave the N.C. terminal disconnected. Shrink the tubing over the joints.
* **Wire the Encoders:** Solder independent signal lines to Phase A and Phase B. Solder a third line to one push-switch terminal, and bridge the middle ground terminal to the second push-switch terminal with a shared wire.
* **Construct Ground Harness:** Tie the ground wires from all encoders and switches together into a unified, clean common ground harness to minimize terminal usage.

### Phase 3: Faceplate Assembly
* **Install Hardware:** Insert the pre-wired switches and encoders through the mounting holes from the back of the front plate.
* **Tighten Fasteners:** Secure the components to the front plate using their included washers and hex nuts. Tighten firmly by hand, avoiding excessive torque on the plastic.
* **Attach Knobs:** Push the interface knobs onto the encoder shafts, aligning any internal flats with the shaft profiles.
* **Join Faceplate to Shell:** Align the completed front plate with the enclosure top. Secure them together using the designated M3 countersunk screws.

### Phase 4: Enclosure Integration
* **Mount the Base Board:** Secure the completed stripboard sub-assembly into the enclosure bottom using the M3 pan-head screws.
* **Terminate Wires:** Route the front-plate wire bundles neatly through the case. Insert each individual signal wire into its designated screw terminal block port and lock down firmly.
* **Seat Microcontroller:** Carefully align the Arduino Micro pins with the female headers on the stripboard. Press down uniformly until fully seated.
* **Attach Mounting Plate:** Fix the mounting plate bracket to the rear of the enclosure bottom using the M4 × 6 mm socket-head screws.
* **Secure Case Shells:** Close the enclosure top and bottom halves together, ensuring no wires are pinched. Lock the shell with the M4 × 26 mm socket-head screws.

## Firmware

The firmware is the little program that runs on the Arduino and makes it act as the game
controller. There are step-by-step guides for loading it — **follow the one for your board:**

- **Arduino Micro:** [firmware/arduino_micro/README.md](firmware/arduino_micro/README.md)
- **Arduino Nano ESP32:** [firmware/arduino_eps32_nano/README.md](firmware/arduino_eps32_nano/README.md)

Both give the board the **"Nobs Autopilot"** name and the custom USB ID (`2341` / `0657`) the app
looks for, and both expose the same 20 buttons in the same order (each encoder's turn-left,
turn-right, and push, then switches SW1–SW8). The knobs are read with full quadrature decoding so
fast turns count correctly, and each knob's acceleration can be tuned from the app and is saved on
the board.

*(Under the hood, the Micro build uses the [Arduino Joystick Library](https://github.com/MHeironimus/ArduinoJoystickLibrary),
bundled as [firmware/arduino_micro/ArduinoJoystickLibrary-2.1.1.zip](firmware/arduino_micro/ArduinoJoystickLibrary-2.1.1.zip);
the Nano ESP32 build uses the ESP32's own USB support.)*

## Nobs FS Companion App

The [**Nobs FS app**](https://github.com/ibovegar/nobs-fs-app) is the companion application for communicating with and configuring this panel. It automatically detects the Nobs Autopilot by its custom USB identity (VID `2341` / PID `0657`), so the right device is selected even when other game controllers are connected.

Use it to:
* **Verify wiring & test inputs:** watch every encoder rotation, encoder push, and switch register live as you turn or press each control, handy for confirming the build before binding anything in the sim.
* **Configure the encoders:** the Settings page sets each encoder's acceleration sensitivity over the USB serial (CDC) port. The value is persisted to the Arduino's EEPROM, so it survives power cycles and applies in the sim too, no re-flashing required.

See the app repository for installation and usage details: <https://github.com/ibovegar/nobs-fs-app>

## Verification & First-Time Setup

1. **Check for shorts first (before plugging in USB):** use a multimeter to make sure none of your
   ground wires are accidentally touching a signal wire. This catches wiring mistakes before they
   reach your PC.
2. **Load the firmware:** follow the guide for your board — [Arduino Micro](firmware/arduino_micro/README.md)
   or [Arduino Nano ESP32](firmware/arduino_eps32_nano/README.md).
3. **Confirm the name:** once plugged in, the panel should show up as `Nobs Autopilot (Vendor: 2341
   Product: 0657)`. If it shows a generic name instead, open Windows **Device Manager**, turn on
   **View → Show hidden devices**, uninstall any old listings for the board, and replug it.
4. **Bind it in the sim:** launch MSFS 2024, go to **Options → Control Options**, pick the
   **Nobs Autopilot** device, and assign your buttons to autopilot commands (the table below is a
   ready-made starting point).

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
