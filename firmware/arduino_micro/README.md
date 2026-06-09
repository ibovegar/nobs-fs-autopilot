# Nobs Autopilot — Arduino Micro Firmware

This is the program (the "firmware") that runs on an **Arduino Micro** board and turns it into a
USB game controller for your flight sim. Once it's loaded, the board shows up to your PC and to
the Nobs app as **"Nobs Autopilot"** with 20 buttons (4 knobs + 8 switches).

> **Which board do you have?** This page is for the **Arduino Micro**. If you're using an
> **Arduino Nano ESP32**, follow [the Nano ESP32 guide](../arduino_eps32_nano/README.md) instead.

For the board to show up as "Nobs Autopilot" (and be picked up automatically by the Nobs app),
it needs to report a specific **USB ID** — two numbers the PC uses to recognise it (`2341` and
`0657`). There's a one-time setup to make that happen, then loading the firmware is easy.

---

## Step 1 — One-time setup: give the board its name

This step tells the Arduino IDE to build the firmware with the right name and USB ID. You only
have to do this **once** on your computer.

1. **Close the Arduino IDE completely.**
2. Open File Explorer, click the address bar, paste this in, and press Enter:
   ```
   %LOCALAPPDATA%\Arduino15\packages\arduino\hardware\avr\
   ```
3. Open the folder you see there (a version number, e.g. `1.8.6`).
4. Open the file **`boards.txt`** with Notepad.
5. Press `Ctrl+F`, search for **`micro.name`** (it's around line 290), and replace those few
   lines with this:
   ```
   micro.name=Nobs Autopilot
   micro.build.vid=0x2341
   micro.build.pid=0x0657
   micro.build.usb_product="Nobs Autopilot"
   ```
6. **Save** the file and reopen the Arduino IDE.

> This only changes your own computer's Arduino setup. It doesn't change the project files, and
> anyone you give a finished, already-flashed board to doesn't need to do any of this.

---

## Step 2 — Load the firmware

1. Open `arduino_micro.ino` (in this folder) in the Arduino IDE.
2. Go to **Tools → Board** and select **Nobs Autopilot** (that's the Arduino Micro — it shows the
   new name now because of Step 1).
3. Plug in the board and click **Upload** (the round arrow).

That's it.

---

## Check that it worked

Once connected, the board should introduce itself to your PC as:

```
Nobs Autopilot (Vendor: 2341 Product: 0657)
```

The Nobs app looks for exactly that USB ID (`2341` / `0657`), so it automatically picks the right
controller even if you have other game controllers plugged in.

> **Seeing a generic name instead?** Open Windows **Device Manager**, turn on **View → Show
> hidden devices**, right-click any old/stale listing for the board and **Uninstall device**,
> then unplug and replug it so Windows reads the new name fresh.
