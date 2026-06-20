# Wiring Map — Which Wire Goes Where

This table tells you which pin on the board each wire connects to, for the **Arduino Nano ESP32**.

**How to read a row:** find the part (e.g. "Encoder 1"), look at what it's connecting ("Phase A
Terminal"), then run a wire from that terminal to the pin listed in the board's column. Pin
names like `D13` or `A0` are printed on the board itself. Every part also connects to **GND**
(ground), see the last row.

The "Virtual HID Button" column is just for reference: it's the button number the sim sees when
you use that control. You don't wire anything for it.

> 💡 Each control has one wire to its signal pin (from this table) and one wire to **GND**. No
> resistors or extra parts needed — the firmware handles that.

| Component Group | Component Label | Hardware Connection | Arduino Nano ESP32 Pin | Virtual HID Button | Action Type |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Encoder 1** | Encoder 1 | Phase A Terminal | **D13** | Button 1 | CW Pulse |
| | Encoder 1 | Phase B Terminal | **A0** | Button 2 | CCW Pulse |
| | Encoder 1 | Switch Terminal 1 | **A1** | Button 3 | Push |
| **Encoder 2** | Encoder 2 | Phase A Terminal | **A2** | Button 4 | CW Pulse |
| | Encoder 2 | Phase B Terminal | **A3** | Button 5 | CCW Pulse |
| | Encoder 2 | Switch Terminal 1 | **A4** | Button 6 | Push |
| **Encoder 3** | Encoder 3 | Phase A Terminal | **A5** | Button 7 | CW Pulse |
| | Encoder 3 | Phase B Terminal | **A6** | Button 8 | CCW Pulse |
| | Encoder 3 | Switch Terminal 1 | **A7** | Button 9 | Push |
| **Encoder 4** | Encoder 4 | Phase A Terminal | **D0** | Button 10 | CW Pulse |
| | Encoder 4 | Phase B Terminal | **D1** | Button 11 | CCW Pulse |
| | Encoder 4 | Switch Terminal 1 | **D3** | Button 12 | Push |
| **Switches** | Switch 1 (SW1) | Terminal 2 (N.O.) | **D12** | Button 13 | Push |
| | Switch 2 (SW2) | Terminal 2 (N.O.) | **D11** | Button 14 | Push |
| | Switch 3 (SW3) | Terminal 2 (N.O.) | **D10** | Button 15 | Push |
| | Switch 4 (SW4) | Terminal 2 (N.O.) | **D9** | Button 16 | Push |
| | Switch 5 (SW5) | Terminal 2 (N.O.) | **D8** | Button 17 | Push |
| | Switch 6 (SW6) | Terminal 2 (N.O.) | **D7** | Button 18 | Push |
| | Switch 7 (SW7) | Terminal 2 (N.O.) | **D6** | Button 19 | Push |
| | Switch 8 (SW8) | Terminal 2 (N.O.) | **D5** | Button 20 | Push |
| **Ground Loop** | All Components | Common / GND Terminals | **GND** | *None* | System Ground |
