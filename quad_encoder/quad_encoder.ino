#include <Encoder.h>

// Motor control pins
const int M1_RPWM = 5;
const int M1_LPWM = 6;
const int M2_RPWM = 7;
const int M2_LPWM = 8;
const int M3_RPWM = 9;
const int M3_LPWM = 10;
const int M4_RPWM = 11;
const int M4_LPWM = 12;
const int ENABLE_PIN = 41;

// Encoder pins (corrected)
const int M1_ENCODER_A = 26;
const int M1_ENCODER_B = 27;
const int M2_ENCODER_A = 30;
const int M2_ENCODER_B = 31;
const int M3_ENCODER_A = 24;
const int M3_ENCODER_B = 25;
const int M4_ENCODER_A = 28;
const int M4_ENCODER_B = 29;

// Encoder objects
Encoder M1_ENCODER(M1_ENCODER_A, M1_ENCODER_B);
Encoder M2_ENCODER(M2_ENCODER_A, M2_ENCODER_B);
Encoder M3_ENCODER(M3_ENCODER_A, M3_ENCODER_B);
Encoder M4_ENCODER(M4_ENCODER_A, M4_ENCODER_B);

long old_position = -999;

void setup() {
  Serial.begin(9600);
  Serial.println("TB7960 + Encoder Test");

  // Enable motor drivers
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);

  // Set motor pins
  int motorPins[] = {
    M1_RPWM, M1_LPWM, M2_RPWM, M2_LPWM,
    M3_RPWM, M3_LPWM, M4_RPWM, M4_LPWM
  };

  for (int i = 0; i < 8; i++) {
    pinMode(motorPins[i], OUTPUT);
    analogWrite(motorPins[i], 0);
  }
}

void spin_motor(int motor, int speed) {
  int rpwm, lpwm;

  switch (motor) {
    case 1: rpwm = M1_RPWM; lpwm = M1_LPWM; break;
    case 2: rpwm = M2_RPWM; lpwm = M2_LPWM; break;
    case 3: rpwm = M3_RPWM; lpwm = M3_LPWM; break;
    case 4: rpwm = M4_RPWM; lpwm = M4_LPWM; break;
    default: return;
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

void read_encoder(Encoder &encoder_object) {
  long new_position = encoder_object.read();
  if (new_position != old_position) {
    old_position = new_position;
    Serial.println(new_position);
  }
}

void loop() {
  // Test Motor 1 spin and encoder reading
  spin_motor(4, -100);
  read_encoder(M4_ENCODER);
  delay(50);
}

