#include <ESP32Servo.h>

// L298N পিন সেটআপ
const int ENA = 12; const int IN1 = 13; const int IN2 = 14;
const int ENB = 27; const int IN3 = 26; const int IN4 = 25;

// রিসিভার পিন
const int CH1_PIN = 33; // Turn
const int CH2_PIN = 32; // Forward/Back
const int CH3_PIN = 18; // Y-Servo
const int CH4_PIN = 19; // X-Servo
const int CH5_PIN = 34; // Pump Switch

// পাম্প রিলে
const int RELAY_PIN = 23;

Servo servoX;
Servo servoY;

void setup() {
  Serial.begin(115200); // সিরিয়াল মনিটরের জন্য
  
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  pinMode(CH1_PIN, INPUT); pinMode(CH2_PIN, INPUT);
  pinMode(CH3_PIN, INPUT); pinMode(CH4_PIN, INPUT);
  pinMode(CH5_PIN, INPUT);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  servoX.setPeriodHertz(50); 
  servoY.setPeriodHertz(50);
  servoX.attach(CH4_PIN, 500, 2400); 
  servoY.attach(CH3_PIN, 500, 2400);
}

void loop() {
  int ch1 = pulseIn(CH1_PIN, HIGH); 
  int ch2 = pulseIn(CH2_PIN, HIGH); 
  int ch3 = pulseIn(CH3_PIN, HIGH); 
  int ch4 = pulseIn(CH4_PIN, HIGH); 
  int ch5 = pulseIn(CH5_PIN, HIGH); 

  // --- সিরিয়াল মনিটরে ডেটা দেখার অংশ (Debugging) ---
  Serial.print("CH1(T):"); Serial.print(ch1);
  Serial.print(" | CH2(S):"); Serial.print(ch2);
  Serial.print(" | CH5(Sw):"); Serial.print(ch5);
  
  int speed = 0;
  String moveStatus = "STOP"; // মুভমেন্ট স্ট্যাটাস দেখার জন্য

  if (ch2 > 1550) { 
    speed = map(ch2, 1550, 2000, 0, 255); 
    moveForward(speed);
    moveStatus = "FORWARD";
  } 
  else if (ch2 < 1450) { 
    speed = map(ch2, 1450, 1000, 0, 255); 
    moveBackward(speed);
    moveStatus = "BACKWARD";
  } 
  else if (ch1 > 1550) { 
    turnRight(200);
    moveStatus = "RIGHT";
  }
  else if (ch1 < 1450) { 
    turnLeft(200);
    moveStatus = "LEFT";
  }
  else {
    stopRobot();
  }

  // পাম্প ও সার্ভো লজিক
  if (ch4 > 900) servoX.write(map(ch4, 1000, 2000, 0, 180));
  if (ch3 > 900) servoY.write(map(ch3, 1000, 2000, 0, 180));

  if (ch5 > 1500) {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.print(" | PUMP: ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    Serial.print(" | PUMP: OFF");
  }

  Serial.print(" | Action: "); Serial.println(moveStatus);

  delay(100); // মনিটরে পড়ার সুবিধার জন্য ডিলে বাড়ানো হয়েছে
}

// মোটর ফাংশনগুলো আগের মতোই...
void moveForward(int s) {
  analogWrite(ENA, s); analogWrite(ENB, s);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void moveBackward(int s) {
  analogWrite(ENA, s); analogWrite(ENB, s);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}
void turnRight(int s) {
  analogWrite(ENA, s); analogWrite(ENB, s);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}
void turnLeft(int s) {
  analogWrite(ENA, s); analogWrite(ENB, s);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void stopRobot() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
}
