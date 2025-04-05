// Mechatronics - Updated for TB7960 drivers and Teensy 4.1

const int ENABLE_PIN = 41;

// Motor control pins
const int M1_RPWM = 5;
const int M1_LPWM = 6;
const int M2_RPWM = 7;
const int M2_LPWM = 8;
const int M3_RPWM = 9;
const int M3_LPWM = 10;
const int M4_RPWM = 11;
const int M4_LPWM = 12;

void setup() {
  // Set all motor pins as output
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH); // Enable all motor drivers

  int motorPins[] = {
    M1_RPWM, M1_LPWM, M2_RPWM, M2_LPWM,
    M3_RPWM, M3_LPWM, M4_RPWM, M4_LPWM
  };

  for (int i = 0; i < 8; i++) {
    pinMode(motorPins[i], OUTPUT);
    analogWrite(motorPins[i], 0); // Initially stop motors
  }
}

// Spins a motor with speed (-255 to 255)
void spin_motor(int motor, int speed) {
  int rpwm, lpwm;

  switch (motor) {
    case 1: rpwm = M1_RPWM; lpwm = M1_LPWM; break;
    case 2: rpwm = M2_RPWM; lpwm = M2_LPWM; break;
    case 3: rpwm = M3_RPWM; lpwm = M3_LPWM; break;
    case 4: rpwm = M4_RPWM; lpwm = M4_LPWM; break;
    default: return; // invalid motor
  }

  if (speed > 0) {
    analogWrite(rpwm, speed);
    analogWrite(lpwm, 0);
  } else if (speed < 0) {
    analogWrite(rpwm, 0);
    analogWrite(lpwm, -speed);
  } else {
    analogWrite(rpwm, 0);
    analogWrite(lpwm, 0);
  }
}

void loop() {
  // Example: spin motors forward and backward
  spin_motor(1, 100);   // Motor 1 forward
  spin_motor(2, -100);  // Motor 2 reverse
  spin_motor(3, 100);   // Motor 3 forward
  spin_motor(4, -100);  // Motor 4 reverse
  delay(2000);

  // Stop all motors
  spin_motor(1, 0);
  spin_motor(2, 0);
  spin_motor(3, 0);
  spin_motor(4, 0);
  delay(2000);
}

