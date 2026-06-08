# Bill of Materials

Complete hardware component list for the Nobs Autopilot — all core electronics, switches, connectors, and structural hardware required to assemble and house the 20-input native USB HID autopilot panel.

## Core Electronics & Switches

| Item # | Qty | Component Description | Manufacturer / Part Number | Purpose / Application |
| :--- | :--- | :--- | :--- | :--- |
| **1.1** | 1 | Arduino Micro (with headers) | Arduino / A000053 | Main microcontroller (ATmega32U4, 5V, USB HID, VID 0x2341 / PID 0x0657) |
| **1.2** | 4 | High-Reliability Mechanical Rotary Encoder | Bourns / PEC11H-4020F-S0016 | Multi-turn cockpit adjustments (HDG, ALT, V/S, SPD) |
| **1.3** | 8 | Subminiature SPDT Momentary Toggle Switch | E-Switch / 700SP7B10M2REH | Panel engagement buttons (SW1–SW8) |
| **1.4** | 1 | Stripboard Prototype Board | BusBoard Prototype Systems / StripBoard-1 | Main sub-assembly chassis carrier board |
| **1.5** | 2 | Female 2.54 mm Pin Header, 26 positions | — | Sockets soldered to the stripboard to seat the Arduino Micro |
| **1.6** | 8 | Fixed Terminal Block (2.54 mm Pitch) | GCT / TBC05-06-1-G-G | Clean screw-down wiring for switches and encoders |

## Knobs, Caps, Connectors & Wiring

| Item # | Qty | Component Description | Specifications / Notes | Purpose / Application |
| :--- | :--- | :--- | :--- | :--- |
| **2.1** | 4 | Rotary Encoder Knob, 14 × 14.5 mm | Same Sky / SAK-023 | Physical interface dials for the 4 rotary encoders |
| **2.2** | 8 | Button Cap | C&K / 798209000 | Caps for the 700-series momentary switches |
| **2.3** | 1 | USB Micro-B to USB-A Cable | 1.5 m to 2 m, data-sync capable | Connects the Arduino Micro to your PC flight simulator setup |
| **2.4** | 1 | 24 AWG Solid Core Hook-Up Wire Kit | Multi-color pack (black, red, blue, yellow) | Internal point-to-point wiring (signals and ground loops) |
| **2.5** | 1 | Heat Shrink Tubing Assortment Pack | 1.5 mm to 3.5 mm diameters | Insulates soldered connections on the encoder and switch lugs |

## Structural & Enclosure Hardware

| Item # | Qty | Component Description | Specifications / Notes | Purpose / Application |
| :--- | :--- | :--- | :--- | :--- |
| **3.1** | 1 | Enclosure Top | 3D Printed (PLA/PETG) | Main upper housing body shell |
| **3.2** | 1 | Enclosure Bottom | 3D Printed (PLA/PETG) | Main lower housing base structure |
| **3.3** | 1 | Mounting Plate | 3D Printed (PLA/PETG) | Interface bracket to attach the panel to your desk/rig |
| **3.4** | 1 | Front Plate | 3D Printed (PLA/PETG) | Visual faceplate holding switches and text labels |
| **3.5** | 13 | M3 Heat-Set Insert | Brass, press-fit | Threaded anchors moulded into the 3D-printed parts |
| **3.6** | 10 | M4 Heat-Set Insert | Brass, press-fit | Threaded anchors moulded into the 3D-printed parts |
| **3.7** | 9 | M3 × 10 mm Countersunk Screw | Flat head machine screws | Fastens the front plate flat against the enclosure frame |
| **3.8** | 4 | M3 × 6 mm Pan-Head Screw | Rounded button head | Fastens the internal circuit board assembly to its mounts |
| **3.9** | 6 | M4 × 6 mm Socket-Head Screw | Hex drive cap screws | Fastens the mounting plate interface to the enclosure body |
| **3.10** | 4 | M4 × 26 mm Socket-Head Screw | Hex drive long cap screws | Clamps the enclosure top and bottom halves together |

## Tools Recommended for Assembly (Not in BOM Cost)
* **Soldering Iron & Lead-Free Rosin-Core Solder:** Essential for attaching wires to the E-Switch gold terminals and Bourns pins.
* **Wire Strippers & Flush Cutters:** For clean trimming and prepping of your 24 AWG signal cables.
* **Soldering Iron / Insert Tool for Heat-Set Inserts:** For pressing the brass inserts squarely into the 3D-printed parts.
* **Multimeter:** Crucial for testing continuity and checking for short circuits across your common ground loops before plugging into USB power.
</content>
