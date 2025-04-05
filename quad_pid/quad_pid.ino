// Dead range is ~2.25 dgree or 18-23 PWM PID = 0.1,0.01,0.05 (No load)
//Done for M1(front left), can be translated to all others too)

#include <Encoder.h>

// ===================== PIN DEFINITIONS =====================
const int M1_RPWM = 5;
const int M1_LPWM = 6;
const int ENABLE_PIN = 41;

// Quadrature encoder pins
const int M1_ENCODER_A = 26;
const int M1_ENCODER_B = 27;

// For a 50:1 Pololu 37D with 64 CPR => 3200 counts/rev at the output.
const long COUNTS_PER_REV = 3200;

// Flip this if applying positive PWM was causing negative counts:
bool M1_REVERSE = true;  

Encoder M1_Enc(M1_ENCODER_A, M1_ENCODER_B);

// ===================== PID GAINS =====================
// Tune as needed
float Kp = 0.1f; 
float Ki = 0.01f; 
float Kd = 0.05f;

// We'll move 90° => +800 counts from current
long targetPosition = 0;

// PID state
float errorIntegral = 0.0f;  
long lastError = 0;

// For timing
unsigned long lastLoopTime = 0;

// Clamp parameters
const int MAX_PWM = 100;     // limit motor power
const float MAX_INTEGRAL = 3000.0f;  // avoid integral wind-up

// -----------------------------------------------------------------------------
// HELPER FUNCTIONS
// -----------------------------------------------------------------------------
long getMotorEncoderCount() {
  long raw = M1_Enc.read();
  return (M1_REVERSE ? -raw : raw);
}

void driveMotor(int pwmVal) {
  // Constrain to ±MAX_PWM
  if (pwmVal >  MAX_PWM) pwmVal =  MAX_PWM;
  if (pwmVal < -MAX_PWM) pwmVal = -MAX_PWM;

  if (pwmVal >= 0) {
    analogWrite(M1_RPWM, pwmVal);
    analogWrite(M1_LPWM, 0);
  } else {
    analogWrite(M1_RPWM, 0);
    analogWrite(M1_LPWM, -pwmVal); 
  }
}

// -----------------------------------------------------------------------------
// SETUP
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH); // Enable driver

  pinMode(M1_RPWM, OUTPUT);
  pinMode(M1_LPWM, OUTPUT);

  // Reset encoder to zero, if desired
  M1_Enc.write(0);

  // We'll do a 90° move => 800 counts from current position
  long startPos = getMotorEncoderCount();
  targetPosition = startPos + 800;  // ~90 degrees forward

  // IMPORTANT: Initialize lastError so first derivative isn't huge
  long initialError = (targetPosition - startPos);
  lastError = initialError;

  lastLoopTime = millis();

  Serial.println("Setup complete. Using full PID to move motor ~90 degrees.");
  Serial.print("Current: ");
  Serial.print(startPos);
  Serial.print(", Target: ");
  Serial.println(targetPosition);
}

// -----------------------------------------------------------------------------
// MAIN LOOP
// -----------------------------------------------------------------------------
void loop() {
  // Calculate time step
  unsigned long now = millis();
  float dt = (now - lastLoopTime) / 1000.0f;
  lastLoopTime = now;

  // Avoid any potential divide by zero
  if (dt < 0.000001f) dt = 0.000001f;

  // Read current position
  long currentPos = getMotorEncoderCount();

  // Error
  long error = targetPosition - currentPos;

  // PID: Integral
  errorIntegral += (error * dt);
  // Clamp integral
  if (errorIntegral >  MAX_INTEGRAL) errorIntegral =  MAX_INTEGRAL;
  if (errorIntegral < -MAX_INTEGRAL) errorIntegral = -MAX_INTEGRAL;

  // Derivative
  float dError = (error - lastError) / dt;
  lastError = error;

  // Weighted sum
  float pidOutput = (Kp * error) + (Ki * errorIntegral) + (Kd * dError);

  // Convert to int and drive
  int pwmVal = (int)pidOutput;
  driveMotor(pwmVal);

  // Debug every ~0.5s
  static unsigned long lastPrint = 0;
  if (now - lastPrint > 500) {
    lastPrint = now;
    Serial.print("Pos=");
    Serial.print(currentPos);
    Serial.print(", Err=");
    Serial.print(error);
    Serial.print(", Int=");
    Serial.print(errorIntegral);
    Serial.print(", dE=");
    Serial.print(dError);
    Serial.print(", PWM=");
    Serial.println(pwmVal);
  }

  delay(20); // ~50 Hz
}



