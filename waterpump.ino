#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= MOTOR A (NEW MOTOR - OUT1/OUT2) =================
const int ENA = 10;
const int IN1 = 7;
const int IN2 = 4;

// ================= MOTOR B (SUCKER MOTOR - OUT3/OUT4) =================
const int IN3 = 5;
const int IN4 = 6;
const int ENB = 9;

// ================= SENSORS =================
const int IR_SUCKER_PIN = 2;    // Controls Motor B
const int IR_NEW_PIN = 11;      // Controls Motor A
const int FLOAT_PIN = 3;        // Water safety switch
const int BUZZER_PIN = 8;

// ================= TIMING CONFIG =================
const unsigned long MOTOR_A_RUN_TIME = 500; // New motor time (ms)
const unsigned long MOTOR_B_RUN_TIME = 250; // Sucker motor time (half)
const unsigned long BUZZER_TIME     = 2000; // Alarm time

// ================= STATE VARIABLES =================
bool systemLocked = false;

// Motor A
bool motorA_Running = false;
bool lastIR_A_State = HIGH;
unsigned long motorA_StartTime = 0;

// Motor B
bool motorB_Running = false;
bool lastIR_B_State = HIGH;
unsigned long motorB_StartTime = 0;

void setup() {
  // Motor pins
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);

  // Sensor pins
  pinMode(IR_SUCKER_PIN, INPUT);
  pinMode(IR_NEW_PIN, INPUT);
  pinMode(FLOAT_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Dual Motor Sys");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(1500);
  lcd.clear();
}

void loop() {
  // -------- Read Sensors --------
  bool irSuckerVal = digitalRead(IR_SUCKER_PIN); // LOW = Blocked
  bool irNewVal    = digitalRead(IR_NEW_PIN);    // LOW = Blocked
  bool waterOK     = !digitalRead(FLOAT_PIN);    // LOW = Water OK

  // ================= SAFETY LOCK =================
  if (!waterOK) {
    stopMotorA();
    stopMotorB();
    systemLocked = true;

    lcd.setCursor(0, 0); lcd.print("Water: LOW!     ");
    lcd.setCursor(0, 1); lcd.print("System: LOCKED  ");

    digitalWrite(BUZZER_PIN, HIGH);
    delay(BUZZER_TIME);
    digitalWrite(BUZZER_PIN, LOW);

    lastIR_A_State = irNewVal;
    lastIR_B_State = irSuckerVal;
    return;
  }

  // ================= RESTORE SYSTEM =================
  if (waterOK && systemLocked) {
    systemLocked = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Water Restored");
    delay(1000);
    lcd.clear();
  }

  // ================= MOTOR A LOGIC =================
  if (!motorA_Running && lastIR_A_State == HIGH && irNewVal == LOW) {
    startMotorA();
  }

  if (motorA_Running && (millis() - motorA_StartTime >= MOTOR_A_RUN_TIME)) {
    stopMotorA();
  }

  lastIR_A_State = irNewVal;

  // ================= MOTOR B LOGIC =================
  if (!motorB_Running && lastIR_B_State == HIGH && irSuckerVal == LOW) {
    startMotorB();
  }

  if (motorB_Running && (millis() - motorB_StartTime >= MOTOR_B_RUN_TIME)) {
    stopMotorB();
  }

  lastIR_B_State = irSuckerVal;

  // ================= LCD STATUS =================
  lcd.setCursor(0, 0);
  lcd.print("New:");
  lcd.print(motorA_Running ? "ON " : "OFF");
  lcd.print("  Suc:");
  lcd.print(motorB_Running ? "ON " : "OFF");

  lcd.setCursor(0, 1);
  lcd.print(irNewVal == LOW ? "IR1:DET " : "IR1:--- ");
  lcd.print(irSuckerVal == LOW ? "IR2:DET " : "IR2:--- ");

  delay(50);
}

// ================= MOTOR FUNCTIONS =================

void startMotorA() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 200);
  motorA_Running = true;
  motorA_StartTime = millis();
}

void stopMotorA() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  motorA_Running = false;
}

void startMotorB() {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 200);
  motorB_Running = true;
  motorB_StartTime = millis();
}

void stopMotorB() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
  motorB_Running = false;
}




