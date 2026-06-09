// Nobs Autopilot — Arduino Nano ESP32 (ESP32-S3) USB HID gamepad firmware.
//
// Functionally identical to the Arduino Micro build, ported to the ESP32-S3's
// native USB. Exposes 20 buttons in the exact order the nobs-fs app expects:
//   buttons  0..11  → 4 encoders × (CW, CCW, push)
//   buttons 12..19  → 8 standalone switches (SW1..SW8)
// See docs/arduino-esp-32-wiring.md for the full button + pin table.
//
// Host config: the app's Settings page sets encoder acceleration sensitivity over
// the USB CDC serial port — see the serial protocol near handleSerialConfig().
//
// ── Arduino IDE Tools settings (required) ─────────────────────────────────────
//   Board:        "Arduino Nano ESP32"
//   USB Mode:     "Normal mode (TinyUSB)"    ← the default; needed for custom HID
//   Pin Numbers:  "By Arduino pin (default)"  so D0/A0… match the wiring doc
// No other Tools changes and NO edits to the installed Arduino core are needed — the
// whole USB-identity fix lives in this repo (this sketch + build_opt.h beside it).
//
// ── USB identity (set at runtime; enabled by build_opt.h) ─────────────────────
// This device must enumerate as VID 0x2341 / PID 0x0657 / "Nobs Autopilot" so the
// app's gamepad filter selects it (same identity as the Micro build). setup() sets
// that via USB.VID()/PID()/productName() before USB.begin().
//
// That only works because the Nano ESP32 normally forces "USB CDC On Boot" and "DFU On
// Boot" on, which would make the core bring USB up at boot (before setup()), freezing
// the descriptor with the stock 0x2341/0x0070 identity. The build_opt.h file next to
// this sketch overrides both to 0 (it is appended to every compile, including the
// core's), so the boot-time USB.begin() is skipped and our runtime identity wins.
//
// Because "CDC On Boot" is then off, the core's `Serial` is the hardware UART — so the
// app's config port is our own USB CDC interface (the USBSerial object below).
//
// Re-flashing: the running firmware reports a custom PID (and, with DFU-on-boot off, no
// DFU runtime interface), so the IDE can't auto-enter the bootloader. Double-tap the
// RESET button (RGB LED pulses) before uploading. See this folder's README.md.

#include "USB.h"
#include "USBHID.h"
#include "USBCDC.h"
#include <EEPROM.h>

#if ARDUINO_USB_MODE
#error "Set Tools > USB Mode to 'Normal mode (TinyUSB)'. Custom HID needs the TinyUSB stack."
#endif
#if ARDUINO_USB_CDC_ON_BOOT
#error "build_opt.h is not taking effect: ARDUINO_USB_CDC_ON_BOOT must be 0. Make sure build_opt.h sits next to this .ino (it disables CDC/DFU-on-boot so the runtime USB identity works)."
#endif

// ── Custom HID gamepad: 20 buttons, no axes ───────────────────────────────────
// The Micro used the ArduinoJoystickLibrary; here we declare an equivalent report
// descriptor (20 1-bit buttons + 4 bits of padding = a flat 3-byte report) and
// drive it through the ESP32 core's USBHID device interface.
static const uint8_t HID_REPORT_DESCRIPTOR[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop)
  0x09, 0x05,        // Usage (Game Pad)
  0xA1, 0x01,        // Collection (Application)
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (Button 1)
  0x29, 0x14,        //   Usage Maximum (Button 20)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x14,        //   Report Count (20)
  0x81, 0x02,        //   Input (Data,Var,Abs)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x04,        //   Report Count (4)   — padding to a whole byte
  0x81, 0x03,        //   Input (Const,Var,Abs)
  0xC0               // End Collection
};

class NobsGamepad : public USBHIDDevice {
public:
  NobsGamepad() { HID.addDevice(this, sizeof(HID_REPORT_DESCRIPTOR)); }

  void begin() { HID.begin(); }

  // Set/clear one button bit in the pending report (mirrors Joystick.setButton).
  void setButton(uint8_t index, uint8_t value) {
    if (index >= 20) return;
    uint8_t byteIdx = index >> 3;
    uint8_t mask    = 1 << (index & 7);
    if (value) report[byteIdx] |= mask;
    else       report[byteIdx] &= ~mask;
  }

  // One USB report carrying every button change since the last call (mirrors
  // Joystick.sendState). Only transmitted when something actually changed — HID
  // hosts track button state edge-to-edge, so re-sending identical reports just
  // adds USB traffic and risks blocking the loop.
  void sendState() {
    if (memcmp(report, lastSent, sizeof(report)) == 0) return;
    if (HID.SendReport(0, report, sizeof(report))) {
      memcpy(lastSent, report, sizeof(report));
    }
  }

  // USBHIDDevice: hand the descriptor to the core during enumeration.
  uint16_t _onGetDescriptor(uint8_t* buffer) override {
    memcpy(buffer, HID_REPORT_DESCRIPTOR, sizeof(HID_REPORT_DESCRIPTOR));
    return sizeof(HID_REPORT_DESCRIPTOR);
  }

private:
  USBHID  HID;
  uint8_t report[3]   = { 0, 0, 0 };
  uint8_t lastSent[3] = { 0xFF, 0xFF, 0xFF }; // force the first real report out
};

NobsGamepad Joystick;

// USB CDC config port. With CDC-on-boot disabled (via build_opt.h) the core's `Serial`
// is the hardware UART, so we expose our own USB CDC interface for the app's Settings
// page. Like USBHID, USBCDC registers its interface in its constructor (static init),
// so it is in place before USB.begin() builds the composite descriptor in setup().
USBCDC USBSerial;

// ── Pin assignments (Arduino Nano ESP32 labels — see arduino-esp-32-wiring.md) ─
// Encoders: A/B quadrature pins + S push button. Encoder common (C) and the
// extra W pin are wired to GND.
//
// HEADS-UP: Encoder 1 Phase A is on D13, which on the Nano ESP32 is also the
// built-in amber LED (GPIO48 / LED_BUILTIN). INPUT_PULLUP holds it HIGH, so that LED
// stays lit whenever the firmware runs — expected, not a fault. The encoder still
// reads, but if you want the LED off (or cleaner readings), move Phase A to a free
// header pin (D2 or D4 are unused) and update the wiring doc to match.
const uint8_t encA[4]    = { D13, A2, A5, D0 };
const uint8_t encB[4]    = { A0,  A3, A6, D1 };
const uint8_t encPush[4] = { A1,  A4, A7, D3 };

// Standalone switches SW1..SW8 (pin 1 to the MCU, pin 2 to GND).
const uint8_t swPin[8]   = { D12, D11, D10, D9, D8, D7, D6, D5 };

// NOTE: every pin above is driven with INPUT_PULLUP, so the closed-to-GND wiring
// reads LOW = pressed. All of these GPIOs support the ESP32-S3 internal pull-up;
// if a future revision moves a signal onto a pull-up-less pad, add an external
// 10 kΩ pull-up to 3V3 there.

// ── Button index map (must match the wiring doc / src/panel/panel.ts) ─────────
const uint8_t encBase[4] = { 0, 3, 6, 9 }; // CW = base, CCW = base + 1, push = base + 2
const uint8_t swBase     = 12;

// ── Encoder rotation state ────────────────────────────────────────────────────
// Full quadrature decode: every loop read both A and B and accumulate only valid
// Gray-code transitions. Sampling B at a single A edge mis-read direction (CCW
// triggered false CW); here a bounce that jumps two bits, or wiggles back and
// forth, nets to zero — no false steps in either direction, and it doesn't care
// where the detent rests. One detent = one full cycle = 4 counts, so we emit a
// CW/CCW step each time the accumulator crosses ±DETENT_STEPS.
//
// Each detent becomes one or more momentary CW/CCW presses, *queued* rather than
// held: a held-and-re-armed button (the old approach) merged into one long press
// when you spun fast, so rapid rotation lost counts. The generator emits each
// queued press as a pulse (PULSE_ON_MS) + forced low gap (PULSE_GAP_MS) so each
// one is counted as a distinct press.
//
// Pulse length is sized for MSFS, NOT the web app. The app reads every ~1 ms USB
// report, so it counts even 2 ms pulses — but MSFS only samples gamepad state
// about once per frame (~16–33 ms). A pulse shorter than a frame can begin and end
// between two MSFS samples and never register, which is why fast spins moved the
// heading bug barely or not at all. To make MSFS see each press we hold it (and the
// gap after it) for about one frame at 60 fps (~16 ms), capping the emit rate at
// ~1000/(ON+GAP) ≈ 33 presses/s — the practical ceiling, since MSFS counts each
// press as 1° so max heading-bug rate ≈ that many °/s. If the sim runs below 60 fps
// (or stutters), a press can again fall between samples; bump ON/GAP back toward 20
// ms if steps go missing. The queue clamp is small so a fast flick's tail drains in
// ~0.25 s, not seconds.
const int8_t        DETENT_STEPS   = 4;  // quadrature counts per detent
const unsigned long PULSE_ON_MS    = 15; // CW/CCW button held high per press (~1 MSFS frame @ 60 fps)
const unsigned long PULSE_GAP_MS   = 15; // forced low gap between presses (~1 MSFS frame @ 60 fps)
const int8_t        STEP_QUEUE_MAX = 8;  // clamp pending steps (bounds post-spin tail to ~0.25 s)

// Rotational acceleration: presses emitted per detent grow with spin speed (the
// time since the previous detent on that encoder). A slow, deliberate turn stays
// 1:1 for single-unit precision; a fast flick multiplies, so e.g. the MSFS heading
// bug races instead of crawling 1° per detent. Tune the thresholds/multipliers to
// taste. Kept within the emit ceiling so the queue (and its tail) stays small.
const unsigned long ACCEL_T1 = 80; // ms between detents; slower than this → ×1
const unsigned long ACCEL_T2 = 40; // ms
const unsigned long ACCEL_T3 = 20; // ms; faster than this → fastest multiplier
const int8_t        ACCEL_M1 = 2;  // ACCEL_T2..ACCEL_T1 ms apart
const int8_t        ACCEL_M2 = 5;  // ACCEL_T3..ACCEL_T2 ms apart
const int8_t        ACCEL_M3 = 10; // faster than ACCEL_T3 (fast flick)

// Acceleration sensitivity (0..255), *per encoder*, host-configurable and saved in
// emulated EEPROM (flash). It scales the *extra* presses from the curve above: 0 =
// acceleration off (always 1:1), 255 = the full ACCEL_M* multipliers. Each encoder
// i uses EEPROM byte EEPROM_ADDR_ACCEL + i; an uninitialised byte reads 0xFF (=255),
// so the out-of-box default is full acceleration on every knob.
const int     EEPROM_SIZE       = 8;
const int     EEPROM_ADDR_ACCEL = 0;
uint8_t       accelSens[4] = { 255, 255, 255, 255 };

// ESP32's EEPROM is emulated in flash: writes are buffered until commit(). Mirror
// the AVR EEPROM.update() — only touch flash when the value actually changes.
void eepromUpdate(int addr, uint8_t value) {
  if (EEPROM.read(addr) != value) {
    EEPROM.write(addr, value);
    EEPROM.commit();
  }
}

// Direction of each (prevAB << 2 | currAB) transition: +1 CW, -1 CCW, 0 none/invalid.
// Derived from the confirmed orientation: A-falling-with-B-high (11→01) is CW.
const int8_t qdec[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0,
};

uint8_t       prevAB[4];     // last (A<<1 | B) phase per encoder
int8_t        accum[4];      // accumulated quadrature counts toward a detent
int8_t        pending[4];    // queued steps not yet pulsed (signed: + CW, − CCW)
unsigned long lastStepMs[4]; // millis() of the previous detent (for acceleration)
bool          pulseOn[4];    // is a CW/CCW press currently asserted?
int           pulseBtn[4];   // which button that press is on
unsigned long phaseUntil[4]; // millis() when the current on/gap phase ends

// ── Host serial config ───────────────────────────────────────────────────────
// Line protocol over the USB CDC port (the app's Settings page). Each encoder has
// its own acceleration sensitivity, addressed by a single-digit index i (0..3):
//   "A<i><n>\n"  set encoder i sensitivity to n (0..255), persisted to EEPROM
//   "A<i>?\n"    query encoder i sensitivity
// Either way we reply "A<i>=<n>\n" so the host can confirm/read the stored value.
char    cmdBuf[8];
uint8_t cmdLen = 0;

void applyAccelCommand(const char* s) {
  if (s[0] != 'A') return;
  uint8_t i = s[1] - '0';
  if (i >= 4) return;          // need a valid encoder index (0..3)
  const char* rest = s + 2;    // value or '?' follows the index
  if (rest[0] != '?' && rest[0] != '\0') {
    long v = atol(rest);
    if (v < 0) v = 0;
    else if (v > 255) v = 255;
    accelSens[i] = (uint8_t)v;
    eepromUpdate(EEPROM_ADDR_ACCEL + i, accelSens[i]); // only writes if changed
  }
  USBSerial.print('A');
  USBSerial.print(i);
  USBSerial.print('=');
  USBSerial.println(accelSens[i]);
}

void handleSerialConfig() {
  while (USBSerial.available()) {
    char c = USBSerial.read();
    if (c == '\n' || c == '\r') {
      cmdBuf[cmdLen] = '\0';
      if (cmdLen > 0) applyAccelCommand(cmdBuf);
      cmdLen = 0;
    } else if (cmdLen < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdLen++] = c;
    }
    // Overlong tokens (no newline) are dropped once the buffer fills.
  }
}

void setup() {
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(encA[i], INPUT_PULLUP);
    pinMode(encB[i], INPUT_PULLUP);
    pinMode(encPush[i], INPUT_PULLUP);
    prevAB[i] = (digitalRead(encA[i]) << 1) | digitalRead(encB[i]);
    accum[i] = 0;
    pending[i] = 0;
    lastStepMs[i] = 0;
    pulseOn[i] = false;
    pulseBtn[i] = -1;
    phaseUntil[i] = 0;
  }
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(swPin[i], INPUT_PULLUP);
  }
  // Load each encoder's saved acceleration sensitivity (0xFF on a fresh chip → 255 = full).
  EEPROM.begin(EEPROM_SIZE);
  for (uint8_t i = 0; i < 4; i++) accelSens[i] = EEPROM.read(EEPROM_ADDR_ACCEL + i);

  // USB identity — set BEFORE USB.begin() so the descriptor is built with VID 0x2341 /
  // PID 0x0657 / "Nobs Autopilot" (matches the Micro, so the app's gamepad filter picks
  // it up). This works because build_opt.h disabled CDC/DFU-on-boot, so USB has NOT
  // been started yet (see the header).
  USB.VID(0x2341);
  USB.PID(0x0657);
  USB.productName("Nobs Autopilot");
  USB.manufacturerName("Arduino");

  Joystick.begin();        // create the HID report semaphores/mutex
  USBSerial.begin(115200); // USB CDC config port; do NOT wait on it (would stall)
  USB.begin();             // build + start the composite USB device (HID + CDC)
}

void loop() {
  unsigned long now = millis();

  handleSerialConfig(); // apply any pending acceleration-sensitivity command

  // ── Encoders ────────────────────────────────────────────────────────────────
  for (uint8_t i = 0; i < 4; i++) {
    // Accumulate valid quadrature transitions; emit a step each full detent.
    uint8_t ab = (digitalRead(encA[i]) << 1) | digitalRead(encB[i]);
    if (ab != prevAB[i]) {
      accum[i] += qdec[(prevAB[i] << 2) | ab];
      prevAB[i] = ab;

      int8_t dir = 0;
      if (accum[i] >= DETENT_STEPS) {
        dir = 1;            // clockwise  → CW
        accum[i] -= DETENT_STEPS;
      } else if (accum[i] <= -DETENT_STEPS) {
        dir = -1;           // counter-cw → CCW
        accum[i] += DETENT_STEPS;
      }

      if (dir != 0) {
        // Acceleration: scale presses-per-detent by how fast we're turning, then
        // by the host-set sensitivity (0 = off/1:1, 255 = full curve).
        unsigned long delta = now - lastStepMs[i];
        lastStepMs[i] = now;
        int8_t baseMult = (delta < ACCEL_T3) ? ACCEL_M3
                        : (delta < ACCEL_T2) ? ACCEL_M2
                        : (delta < ACCEL_T1) ? ACCEL_M1
                        : 1;
        int16_t mult = 1 + ((int16_t)(baseMult - 1) * accelSens[i]) / 255;
        // Queue the (scaled) steps, clamped; the generator below paces them out.
        int16_t want = pending[i] + (int16_t)dir * mult;
        if (want > STEP_QUEUE_MAX) want = STEP_QUEUE_MAX;
        else if (want < -STEP_QUEUE_MAX) want = -STEP_QUEUE_MAX;
        pending[i] = (int8_t)want;
      }
    }

    // Pulse generator: turn queued steps into distinct press/gap cycles so even
    // rapid rotation counts every detent.
    if (pulseOn[i]) {
      if (now >= phaseUntil[i]) {           // end the press → start the low gap
        Joystick.setButton(pulseBtn[i], 0);
        pulseOn[i] = false;
        phaseUntil[i] = now + PULSE_GAP_MS;
      }
    } else if (now >= phaseUntil[i] && pending[i] != 0) {
      int8_t dir = (pending[i] > 0) ? 1 : -1; // start the next queued press
      pending[i] -= dir;
      pulseBtn[i] = (dir > 0) ? encBase[i] : encBase[i] + 1;
      Joystick.setButton(pulseBtn[i], 1);
      pulseOn[i] = true;
      phaseUntil[i] = now + PULSE_ON_MS;
    }

    // Push button is reported as a held level (pressed = LOW with INPUT_PULLUP).
    Joystick.setButton(encBase[i] + 2, digitalRead(encPush[i]) == LOW ? 1 : 0);
  }

  // ── Standalone switches ───────────────────────────────────────────────────────
  for (uint8_t i = 0; i < 8; i++) {
    Joystick.setButton(swBase + i, digitalRead(swPin[i]) == LOW ? 1 : 0);
  }

  // One USB report per loop with all the updates above (sent only when changed).
  Joystick.sendState();
}
