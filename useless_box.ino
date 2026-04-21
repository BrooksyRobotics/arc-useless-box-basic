/*
  Useless Box V2 — Escalating Reactions
  ─────────────────────────────────────────────────────────────────────────────
  Hardware:
    - ON Button     → D2  (other leg to GND — uses INPUT_PULLUP)
    - OFF Button    → D3  (other leg to GND — uses INPUT_PULLUP)
    - LED           → D4  → 220Ω resistor → GND
    - Servo signal  → D9  (servo red → 5V, brown/black → GND)

  Reactions (escalate with each press within 1 minute):
    Tier 1 — Lazy:          Slow, reluctant sweep
    Tier 2 — Annoyed:       Fast irritated jab
    Tier 3 — Dramatic:      Long tense pause, then slam
    Tier 4 — Fake-out:      Starts, retreats, then actually presses
    Tier 5 — Tantrum:       5 rapid hits, dramatic pause, one final hit
  ─────────────────────────────────────────────────────────────────────────────
*/

#include <Servo.h>

// ── Pin assignments ──────────────────────────────────────────────────────────
const int GREEN_BTN_PIN = 2; // ON
const int RED_BTN_PIN   = 3; // OFF
const int LED_PIN        = 4;
const int SERVO_PIN      = 9;

// ── Servo positions ──────────────────────────────────────────────────────────
// Adjust PRESS_ANGLE until the arm cleanly hits the red button
const int HOME_ANGLE  = 0;
const int PRESS_ANGLE = 90;
const int MID_ANGLE   = PRESS_ANGLE / 2;  // Used for fake-out move

// ── Escalation settings ──────────────────────────────────────────────────────
const unsigned long ESCALATION_WINDOW = 60000;  // 1 minute in milliseconds
int  pressCount      = 0;
unsigned long firstPressTime = 0;

// ── Globals ──────────────────────────────────────────────────────────────────
Servo myServo;
bool  ledState = false;

// ────────────────────────────────────────────────────────────────────────────
void setup() {
  pinMode(GREEN_BTN_PIN, INPUT_PULLUP);
  pinMode(RED_BTN_PIN,   INPUT_PULLUP);
  pinMode(LED_PIN,       OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(HOME_ANGLE);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(9600);
  Serial.println("Useless Box V2 ready. Press the green button.");
}

// ── Helper: sweep servo smoothly between two angles ──────────────────────────
void slowSweep(int fromAngle, int toAngle, int stepDelay) {
  if (fromAngle < toAngle) {
    for (int pos = fromAngle; pos <= toAngle; pos++) {
      myServo.write(pos);
      delay(stepDelay);
    }
  } else {
    for (int pos = fromAngle; pos >= toAngle; pos--) {
      myServo.write(pos);
      delay(stepDelay);
    }
  }
}

// ── TIER 1 — Lazy ────────────────────────────────────────────────────────────
// Slow reluctant sweep, long pause at home before bothering to move
void reaction1() {
  Serial.println("Tier 1: *sigh* Fine.");
  delay(500);                              // Long reluctant pause

  slowSweep(HOME_ANGLE, PRESS_ANGLE, 5);  // Slow crawl to button
  ledState = false;
  digitalWrite(LED_PIN, LOW);              // LED off as arm arrives
  delay(400);

  slowSweep(PRESS_ANGLE, HOME_ANGLE, 5);  // Slow crawl back
}

// ── TIER 2 — Annoyed ─────────────────────────────────────────────────────────
// Short impatient pause, then a fast jab
void reaction2() {
  Serial.println("Tier 2: Really?");
  delay(500);

  myServo.write(PRESS_ANGLE);              // Fast jab
  delay(180);
  ledState = false;
  digitalWrite(LED_PIN, LOW);
  delay(100);

  myServo.write(HOME_ANGLE);              // Snap back
}

// ── TIER 3 — Dramatic ────────────────────────────────────────────────────────
// Tense silence, then a deliberate slam
void reaction3() {
  Serial.println("Tier 3: ..........SLAM.");
  delay(2000);                             // Long tense silence

  myServo.write(PRESS_ANGLE);             // Deliberate slam
  delay(180);
  ledState = false;
  digitalWrite(LED_PIN, LOW);
  delay(100);

  myServo.write(HOME_ANGLE);
}

// ── TIER 4 — Fake-out ────────────────────────────────────────────────────────
// Starts moving, retreats, pauses... then actually does it
void reaction4() {
  Serial.println("Tier 4: Ha — psych. ...okay fine.");

  delay(300);

  // Start moving confidently
  slowSweep(HOME_ANGLE, MID_ANGLE, 12);

  // Change mind, retreat
  delay(400);
  slowSweep(MID_ANGLE, HOME_ANGLE, 12);

  // Pause like it is reconsidering
  delay(700);

  // NOW actually do it, fast
  myServo.write(PRESS_ANGLE);
  delay(180);
  ledState = false;
  digitalWrite(LED_PIN, LOW);
  delay(100);

  myServo.write(HOME_ANGLE);
}

// ── TIER 5 — Tantrum ─────────────────────────────────────────────────────────
// Presses to turn off LED, then 5 rapid angry hits, dramatic pause,
// one final hit for good measure
void reaction5() {
  Serial.println("Tier 5: THAT IS IT. I HAVE HAD ENOUGH.");

  delay(200);

  // First press — turns LED off
  myServo.write(PRESS_ANGLE);
  delay(150);
  ledState = false;
  digitalWrite(LED_PIN, LOW);
  myServo.write(HOME_ANGLE);
  delay(150);

  // 5 rapid tantrum hits (LED already off, arm just keeps going)
  for (int i = 0; i < 5; i++) {
    myServo.write(PRESS_ANGLE);
    delay(100);
    myServo.write(HOME_ANGLE);
    delay(100);
  }

  // Dramatic pause — like it is calming down
  delay(1200);

  // One last hit. Just to be clear.
  myServo.write(PRESS_ANGLE);
  delay(300);
  myServo.write(HOME_ANGLE);

  Serial.println("...and THAT is final.");
}

// ────────────────────────────────────────────────────────────────────────────
void loop() {

  // ── Green button pressed? ─────────────────────────────────────────────────
  if (digitalRead(GREEN_BTN_PIN) == LOW && !ledState) {
    ledState = true;
    digitalWrite(LED_PIN, HIGH);

    unsigned long now = millis();

    // If outside the escalation window, reset the counter
    if (now - firstPressTime > ESCALATION_WINDOW) {
      pressCount     = 0;
      firstPressTime = now;
      Serial.println("--- Escalation window reset ---");
    }

    pressCount++;

    Serial.print("Press #");
    Serial.print(pressCount);
    Serial.print(" (");
    Serial.print((now - firstPressTime) / 1000);
    Serial.println("s into window)");

    // Select reaction tier
    switch (pressCount) {
      case 1:  reaction1(); break;
      case 2:  reaction2(); break;
      case 3:  reaction3(); break;
      case 4:  reaction4(); break;
      default: reaction5(); break;  // Tier 5 repeats for all presses beyond 4
    }

    // Safety: ensure LED is off and arm is home after any reaction
    ledState = false;
    digitalWrite(LED_PIN, LOW);
    myServo.write(HOME_ANGLE);

    delay(300);  // Debounce gap after full cycle
  }

  // ── Red button pressed manually? ─────────────────────────────────────────
  if (digitalRead(RED_BTN_PIN) == LOW && ledState) {
    ledState = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("Red button pressed manually — LED OFF.");
    delay(300);
  }

  delay(20);
}
