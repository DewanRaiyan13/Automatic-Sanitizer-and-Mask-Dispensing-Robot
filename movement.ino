#include <NewPing.h>

// --- Pin Definitions ---
// Motor Pins
const int LeftMotorForward = 7;
const int LeftMotorBackward = 6;
const int RightMotorForward = 5;
const int RightMotorBackward = 4;

// Sensor Pins
#define trig_pin A1 
#define echo_pin A2 

// PIR Sensor Pins
const int PIR_Left_Pin = 8;
const int PIR_Front_Pin = 9;
const int PIR_Right_Pin = 10;

// --- Constants & Variables ---
#define maximum_distance 200
#define stop_distance 20     // Robot stops if human is closer than 20cm

NewPing sonar(trig_pin, echo_pin, maximum_distance); 

void setup(){
  // Motor Pins
  pinMode(RightMotorForward, OUTPUT);
  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);
  
  // PIR Pins
  pinMode(PIR_Left_Pin, INPUT);
  pinMode(PIR_Front_Pin, INPUT);
  pinMode(PIR_Right_Pin, INPUT);
  
  // Optional: Serial for debugging
  Serial.begin(9600);
}

void loop(){
  // 1. Check Distance first (Safety Stop)
  int distance = readPing();
  
  // If object is too close (and valid distance detected), STOP immediately.
  if (distance > 0 && distance < stop_distance) {
    moveStop();
    delay(100); // Small delay to prevent jitter
    return;     // Skip the rest of the loop
  }

  // 2. Read PIR Sensors
  int leftMotion = digitalRead(PIR_Left_Pin);
  int frontMotion = digitalRead(PIR_Front_Pin);
  int rightMotion = digitalRead(PIR_Right_Pin);

  // 3. Movement Logic
  
  // CASE A: Center + Left (Diagonal Left)
  if (frontMotion == HIGH && leftMotion == HIGH) {
    moveDiagonalLeft();
  }
  // CASE B: Center + Right (Diagonal Right)
  else if (frontMotion == HIGH && rightMotion == HIGH) {
    moveDiagonalRight();
  }
  // CASE C: Front Only (Move Forward)
  else if (frontMotion == HIGH) {
    moveForward();
  }
  // CASE D: Left Only (Rotate Left to face target)
  else if (leftMotion == HIGH) {
    turnLeft();
  }
  // CASE E: Right Only (Rotate Right to face target)
  else if (rightMotion == HIGH) {
    turnRight();
  }
  // CASE F: No Motion (Stop and Wait)
  else {
    moveStop();
  }
  
  delay(50); // Short delay for stability
}

// --- Sensor Functions ---

int readPing(){
  int cm = sonar.ping_cm();
  if (cm == 0){
    cm = 250; // Treat 0 (out of range) as far away
  }
  return cm;
}

// --- Movement Functions ---

void moveStop(){
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
  digitalWrite(LeftMotorBackward, LOW);
}

void moveForward(){
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorForward, HIGH);
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorBackward, LOW); 
}

// Standard Turn (Pivot on spot)
void turnRight(){
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);
}

// Standard Turn (Pivot on spot)
void turnLeft(){
  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorForward, HIGH);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
} 

// Diagonal Left: Left motor stops, Right motor moves forward
// This creates a soft curve to the left while moving forward
void moveDiagonalLeft(){
  digitalWrite(LeftMotorForward, LOW); // Stop Left
  digitalWrite(RightMotorForward, HIGH); // Drive Right
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}

// Diagonal Right: Right motor stops, Left motor moves forward
void moveDiagonalRight(){
  digitalWrite(LeftMotorForward, HIGH); // Drive Left
  digitalWrite(RightMotorForward, LOW); // Stop Right
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}

