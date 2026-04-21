# arc-useless-box-basic
The servo doesn't like the LED and "presses the button" to turn it off.

### NOTE: Code turns LED off when servo moves.  Button press was unreliable with servo.

## Components   → Wiring
- ON Button     → D2  (other leg to GND — uses INPUT_PULLUP)
- OFF Button    → D3  (other leg to GND — uses INPUT_PULLUP)
- LED           → D4  → 220Ω resistor → GND
- Servo signal  → D9  (servo red → 5V, brown/black → GND)

<img width="480" height="480" alt="uselessboxgif" src="https://github.com/user-attachments/assets/378682aa-0ea1-4c3a-a305-b1bdbfd69aac" />

