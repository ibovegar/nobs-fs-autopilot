# Nobs Autopilot — Arduino Nano ESP32 Firmware

This is the program (the "firmware") that runs on an **Arduino Nano ESP32** board and turns it
into a USB game controller for Microsoft Flight Simulator. Once it's loaded, the board shows up
to your PC and to the Nobs app as **"Nobs Autopilot"** with **20 buttons**:

| Buttons | What they are |
| :------ | :------------ |
| 12 buttons | 4 rotary knobs (each gives: turn left, turn right, press) |
| 8 buttons | 8 toggle switches |

You don't need to understand any of that to load it — just follow the steps below. If you're
curious how it works, there's a [How it works](#how-it-works-optional-reading) section at the end.

**Files you'll use** (they're all in this folder):
- The firmware: [`arduino_eps32_nano.ino`](./arduino_eps32_nano.ino)
- A small settings file next to it: [`build_opt.h`](./build_opt.h) *(leave it where it is — it's important)*
- Which wire goes where: [`docs/arduino-esp-32-wiring.md`](../../docs/arduino-esp-32-wiring.md)

> 👍 Good news: everything needed is already in this project folder. You do **not** have to edit
> any Arduino program files. Tested with the Arduino ESP32 board package version `2.0.18-arduino.5`.

---

## What you need

- An **Arduino Nano ESP32** board and a USB cable.
- The **Arduino IDE** installed (the free app from arduino.cc).
- This project downloaded to your computer.

---

## Step 1 — Add support for the board

The Arduino IDE doesn't know about the Nano ESP32 until you add it:

1. In the Arduino IDE, open **Tools → Board → Boards Manager…**
2. In the search box type **Arduino ESP32 Boards** and click **Install** on the result by *Arduino*.
   This is a big download, so give it a few minutes.
3. When it's done, go to **Tools → Board → Arduino ESP32 Boards → Arduino Nano ESP32**.

> **If the install fails** with a message like *"platform not installed"*: close the Arduino IDE,
> right-click it and choose **Run as administrator**, then try the install again. (Windows needs
> the extra permission to set up the USB drivers.)

---

## Step 2 — Pick the right settings

In the **Tools** menu, set:

- **USB Mode** → **`Normal mode (TinyUSB)`** (this is usually the default). Don't pick "Debug
  mode (Hardware CDC)".
- **Pin Numbering** → **`By Arduino pin (default)`** so the wiring diagram matches.

That's it — no other settings to change.

---

## Step 3 — Load the firmware onto the board

1. Open `arduino_eps32_nano.ino` (in this folder) in the Arduino IDE.
   *(Keep the `build_opt.h` file in the same folder — the IDE uses it automatically.)*
2. Plug in the board.
3. Click the **Upload** button (the round arrow, top-left).

The **first** time, this just works. 🎉 When it's done, the board reconnects as **Nobs
Autopilot** and you can use it in the app.

---

## Loading it again later (please read — it's different!)

After the firmware is on the board, the board changes its "name tag" to Nobs Autopilot. The
Arduino IDE looks for the *original* Arduino name tag when uploading, so a normal Upload will now
**fail** with a red error like:

```
No DFU capable USB device available
Failed uploading: uploading error: exit status 74
```

This is normal and expected — it just means you need to put the board into its **update mode**
first. Here's how:

1. Find the small **RESET** button on the board.
2. **Press it twice, quickly** — like a fast mouse double-click.
3. Watch the lights: the **yellow/amber light goes off**, and a **green light slowly pulses in
   and out**. That pulsing green light means the board is now waiting for an update. 👍
4. **Now click Upload** in the Arduino IDE. It will load the firmware.
5. When it finishes, **press RESET once** (a single press) to start the new firmware. The
   yellow light comes back on.

> The double-tap takes a little practice — press, then press again within about half a second.
> If the yellow light stays on, it didn't work; just try the rhythm again. You can't break the
> board by doing this.

### If the double-tap won't cooperate (optional, more advanced)

You can load the firmware from the command line instead, which avoids the timing dance. Open
**PowerShell** in the project folder and run:

```powershell
# 1) Build the firmware once (find arduino-cli.exe inside your Arduino IDE install folder):
& "<path to your Arduino IDE>\resources\app\lib\backend\resources\arduino-cli.exe" `
  compile -b arduino:esp32:nano_nora --output-dir .\out firmware\arduino_eps32_nano

# 2) Double-tap RESET so the green light is pulsing, then run:
& "$env:LOCALAPPDATA\Arduino15\packages\arduino\tools\dfu-util\0.11.0-arduino5\dfu-util.exe" `
  --device 0x2341:0x0070 -D .\out\arduino_eps32_nano.ino.bin -Q

# 3) Press RESET once to start the firmware.
```

It worked if you see `Download done.` and `Done!` at the end.

---

## Checking that it works

- **Windows "game controllers" tool:** press `Win`, type **Set up USB game controllers**, open
  it, select the device, click **Properties**, and wiggle your knobs/switches — you should see
  the 20 buttons light up.
- **The Nobs app:** the controller is picked automatically, and the Settings page can connect to
  it.

### Finding it in Windows Device Manager

If you go looking in **Device Manager** and can't find "Nobs Autopilot", don't worry — it's
there, just listed in a confusing way by default. The easy fix:

> In Device Manager, click the **View** menu → **Devices by container**. Now the board shows up
> as a single entry named **Nobs Autopilot**. ✅

(In the normal view, one USB gadget like this gets split into a few separate generic-looking
entries — a "game controller", a "COM port", and a "USB Composite Device" — which is why it can
look like it's missing.)

---

## If something goes wrong

| What you see | What it means | What to do |
| :----------- | :------------ | :--------- |
| *"Platform 'arduino:esp32' not found"* | The board support isn't installed | Do Step 1 again. On Windows, run the IDE **as administrator** if it fails partway. |
| Upload fails: *"No DFU capable USB device available"* or *"exit status 74"* | The board isn't in update mode | Double-tap RESET (wait for the pulsing green light), **then** Upload. See [Loading it again later](#loading-it-again-later-please-read--its-different). |
| Double-tap doesn't seem to do anything | Tricky timing, or you saw the normal 1-second light show at startup | You're in update mode only when the **yellow light is OFF** and the **green light keeps pulsing**. Keep trying the double-press. |
| The app or Windows shows it as **"Unknown device"** | Windows remembered an old, blank name | In Device Manager: **View → Show hidden devices**, right-click the board → **Uninstall device**, then unplug and replug it. |
| A long list of red *"No such file or directory"* errors when compiling | Someone added comments to `build_opt.h` | The `build_opt.h` file must contain **only** the settings line, no `//` comment lines. |

---

## Things that look wrong but are totally fine

- **The browser shows "TinyUSB CDC" or "TinyUSB HID" when picking the device.** The real device
  name *is* "Nobs Autopilot" and the app finds it correctly — this other label is just an
  internal name for one part of the device. It's harmless. (We could rename it, but only by
  editing Arduino's own files, which we avoid so the project stays self-contained.)
- **A yellow light on the board stays on.** That's normal here — one of the knob wires shares the
  board's built-in LED pin, so the light is on whenever the firmware runs. If it bothers you, you
  can move that one wire to pin **D2** or **D4** and update the firmware + wiring diagram to match.

---

## How it works (optional reading)

You can skip this — it's just for the curious or for anyone maintaining the project.

### Why uploading needs the extra steps

A USB device tells your PC who it is using two ID numbers (called **VID** and **PID**). We need
this board to use the *same* IDs as the original Micro version (`2341` / `0657`) so the Nobs app
recognises it the same way.

The tricky part: the Nano ESP32 normally locks in its ID numbers the instant it powers on,
*before* our firmware gets a chance to run — and it also hard-codes the "wrong" ID. So we use a
tiny helper file, **`build_opt.h`**, that sits next to the firmware. The Arduino IDE
automatically applies the settings in that file when it builds, which tells the board *not* to
start USB at power-on. That gives our firmware the opening it needs to set the correct ID, name,
buttons, and serial port itself, the moment it starts. (There's also a built-in safety check
that refuses to build if `build_opt.h` ever goes missing, so the board can never quietly get the
wrong ID.)

Because the board now reports the *custom* ID, the Arduino IDE's normal "auto-restart for
upload" can't find it — which is exactly why re-uploads need the manual double-tap into update
mode.

### What the firmware actually does

- Presents itself as a 20-button game controller (no joysticks/axes).
- Reads the 4 rotary knobs with proper quadrature decoding, and turns each "click" of the knob
  into a quick button press so even fast spinning is counted correctly in MSFS. Each knob also
  has adjustable acceleration (turn faster = bigger jumps), saved on the board.
- Provides a serial port so the Nobs app's Settings page can tweak that acceleration.
- Reads the 8 switches as plain buttons.

The exact pin-to-wire mapping is in the [wiring diagram](../../docs/arduino-esp-32-wiring.md).
