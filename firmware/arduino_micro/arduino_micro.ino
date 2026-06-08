#include <Joystick.h>
#include <EEPROM.h>

// Nobs Autopilot — Arduino Micro (ATmega32U4) USB HID gamepad firmware.
//
// Exposes 20 buttons in the exact order the nobs-fs app expects:
//   buttons  0..11  → 4 encoders × (CW, CCW, push)
//   buttons 12..19  → 8 standalone switches (SW1..SW8)
// See docs/mapping.md for the full button + pin table.
//
// Host config: the app's Settings page sets encoder acceleration sensitivity over
// the USB CDC serial port — see the serial protocol near handleSerialConfig().

Joystick_ Joystick(0x05,
  JOYSTICK_TYPE_GAMEPAD,
  20, 0,                 // 20 buttons, 0 hat switches
  false, false, false,   // no analog axes (X, Y, Z)
  false, false, false,   //                (Rx, Ry, Rz)
  false, false,          //                (rudder, throttle)
  false, false, false);  //                (accelerator, brake, steering)

// ── Pin assignments (Arduino Micro labels) ───────────────────────────────────
// Encoders: A/B quadrature pins + S push button. Encoder common (C) and the
// extra W pin are wired to GND.
const uint8_t encA[4]    = { A0, A3, 3, 0 };   // PF7, PF4, PD0, PD2
const uint8_t encB[4]    = { A1, A4, 2, 1 };   // PF6, PF1, PD1, PD3
const uint8_t encPush[4] = { A2, A5, 4, 16 };  // PF5, PF0, PD4, PB2
//                                        ^ ENC4 push moved off PB0 (D17 = RX LED,
//                                          held low by the LED) onto PB2 (D16 / MOSI).

// Standalone switches SW1..SW8 (pin 1 to the MCU, pin 2 to GND).
const uint8_t swPin[8]   = { 12, 11, 10, 9, 8, 7, 6, 5 };
//                           PD6 PB7 PB6 PB5 PB4 PE6 PD7 PC6

// ── Button index map (must match docs/mapping.md / src/panel/panel.ts) ────────
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
// EEPROM. It scales the *extra* presses from the curve above: 0 = acceleration off
// (always 1:1), 255 = the full ACCEL_M* multipliers. Each encoder i uses EEPROM byte
// EEPROM_ADDR_ACCEL + i; an uninitialised byte reads 0xFF (=255), so the out-of-box
// default is full acceleration on every knob.
const int     EEPROM_ADDR_ACCEL = 0;
uint8_t       accelSens[4] = { 255, 255, 255, 255 };

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
    EEPROM.update(EEPROM_ADDR_ACCEL + i, accelSens[i]); // only writes if changed
  }
  Serial.print('A');
  Serial.print(i);
  Serial.print('=');
  Serial.println(accelSens[i]);
}

void handleSerialConfig() {
  while (Serial.available()) {
    char c = Serial.read();
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
  for (uint8_t i = 0; i < 4; i++) accelSens[i] = EEPROM.read(EEPROM_ADDR_ACCEL + i);
  Serial.begin(115200); // USB CDC; do NOT wait on Serial (would stall the gamepad)

  // Auto-send OFF: otherwise every setButton() blocks on its own USB report (~12
  // per loop), slowing the loop enough to miss quadrature transitions. We batch
  // all updates into one report per loop via sendState() instead.
  Joystick.begin(false);
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

  // One USB report per loop with all the updates above (auto-send is off).
  Joystick.sendState();
}
