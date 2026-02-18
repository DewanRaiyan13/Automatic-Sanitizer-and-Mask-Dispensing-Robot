// Pins
const int pirPin = 2;

const int in1 = 3;
const int in2 = 4;
const int in3 = 5;
const int in4 = 6;

const int trigPin = 8;
const int echoPin = 9;

const long stopDistance = 40; // cm

unsigned long motorStartTime = 0;
const unsigned long motorRunDuration = 2000; // ms
bool motorRunning = false;

void setup() {
  Serial.begin(9600);

  pinMode(pirPin, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  stopMotors();
  delay(3000); // Power bank stabilization
}

void loop() {
  long distance = readUltrasonicDistance();
  //Serial.println(distance);

  // Stop motors if object is too close
  if (distance < stopDistance) {
    stopMotors();
    motorRunning = false; // ignore PIR
    return;
  }

  // PIR Trigger Logic
  int pirState = digitalRead(pirPin);

  if (pirState == LOW && !motorRunning) {
    runMotors();
    motorRunning = true;
    motorStartTime = millis();
  }

  // Stop motors after run duration
  if (motorRunning && millis() - motorStartTime >= motorRunDuration) {
    stopMotors();
    motorRunning = false;
  }
}

// ---------- Motor Functions ----------
void runMotors() {
  digitalWrite(in1,LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// ---------- Ultrasonic Function ----------
long readUltrasonicDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // 30 ms timeout
  long distanceCm = duration * 0.034 / 2;
  return distanceCm;
}


