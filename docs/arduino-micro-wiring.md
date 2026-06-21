# Wiring Map — Which Wire Goes Where

This table tells you which pin on the board each wire connects to, for the **Arduino Micro** (the
older, alternative board — the main build uses the **Arduino Nano ESP32**; see
[arduino-esp-32-wiring.md](arduino-esp-32-wiring.md)).

**How to read a row:** find the part (e.g. "Encoder 1"), look at what it's connecting ("Phase A
Terminal"), then run a wire from that terminal to the pin listed in the board's column. Pin
names like `D3` or `A0` are printed on the board itself. Every part also connects to **GND**
(ground), see the last row.

The "Virtual HID Button" column is just for reference: it's the button number the sim sees when
you use that control. You don't wire anything for it.

> 💡 Each control has one wire to its signal pin (from this table) and one wire to **GND**. No
> resistors or extra parts needed — the firmware handles that.
>
> For a visual reference alongside this table, see the [wiring diagram](wiring-diagram.pdf).

| Component Group | Component Label | Hardware Connection | Arduino Micro Pin | Virtual HID Button | Action Type |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Encoder 1** | Encoder 1 | Phase A Terminal | **A0** | Button 1 | CW Pulse |
| | Encoder 1 | Phase B Terminal | **A1** | Button 2 | CCW Pulse |
| | Encoder 1 | Switch Terminal 1 | **A2** | Button 3 | Push |
| **Encoder 2** | Encoder 2 | Phase A Terminal | **A3** | Button 4 | CW Pulse |
| | Encoder 2 | Phase B Terminal | **A4** | Button 5 | CCW Pulse |
| | Encoder 2 | Switch Terminal 1 | **A5** | Button 6 | Push |
| **Encoder 3** | Encoder 3 | Phase A Terminal | **D3** | Button 7 | CW Pulse |
| | Encoder 3 | Phase B Terminal | **D2** | Button 8 | CCW Pulse |
| | Encoder 3 | Switch Terminal 1 | **D4** | Button 9 | Push |
| **Encoder 4** | Encoder 4 | Phase A Terminal | **D0 (RX)** | Button 10 | CW Pulse |
| | Encoder 4 | Phase B Terminal | **D1 (TX)** | Button 11 | CCW Pulse |
| | Encoder 4 | Switch Terminal 1 | **D16 (MOSI)** | Button 12 | Push |
| **Switches** | Switch 1 (SW1) | Terminal 2 (N.O.) | **D12** | Button 13 | Push |
| | Switch 2 (SW2) | Terminal 2 (N.O.) | **D11** | Button 14 | Push |
| | Switch 3 (SW3) | Terminal 2 (N.O.) | **D10** | Button 15 | Push |
| | Switch 4 (SW4) | Terminal 2 (N.O.) | **D9** | Button 16 | Push |
| | Switch 5 (SW5) | Terminal 2 (N.O.) | **D8** | Button 17 | Push |
| | Switch 6 (SW6) | Terminal 2 (N.O.) | **D7** | Button 18 | Push |
| | Switch 7 (SW7) | Terminal 2 (N.O.) | **D6** | Button 19 | Push |
| | Switch 8 (SW8) | Terminal 2 (N.O.) | **D5** | Button 20 | Push |
| **Ground Loop** | All Components | Common / GND Terminals | **GND** | *None* | System Ground |

> **Pin notes:**
> * Encoder 4's push switch sits on **D16 (MOSI)** rather than the adjacent D17 — D17 drives the on-board RX LED, which holds the line low and would register a phantom press.
> * Encoder 4's Phase A/B share the **D0/D1** UART pins. These stay free for digital I/O because the Arduino Micro talks to the host over native USB CDC, not the hardware UART.
