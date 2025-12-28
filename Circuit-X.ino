#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// Motor Driver Pins
#define IN1 26
#define IN2 27
#define IN3 14
#define IN4 12
#define ENA 25
#define ENB 33

// Ultrasonic Sensor Pins
#define TRIG 32
#define ECHO 13

// IR Sensor Pins (for Lock Mode)
#define LEFT_IR 34
#define RIGHT_IR 35

// Light & Buzzer Pins
#define HEADLIGHT_LEFT 15
#define HEADLIGHT_RIGHT 2
#define TAILLIGHT_LEFT 18
#define TAILLIGHT_RIGHT 5
#define BUZZER 4

// --- OPTIMIZATION --- Global variables to track current motor speed for ramping
int currentLeftSpeed = 0;
int currentRightSpeed = 0;

// Bool Definitions
bool isPowerOn = false;
bool leftIndicatorActive = false;
bool rightIndicatorActive = false;
unsigned long indicatorPreviousMillis = 0;
const long indicatorInterval = 400;
bool indicatorState = false;
bool isEBrakeActive = false;
int speedA = 160;
int speedB = 160;

// Setup Loop
void setup() {
  // --- STABILITY FIX --- Add a delay to allow power to stabilize before any code runs.
  delay(1000); 

  Serial.begin(115200);
  SerialBT.begin("Sight GECBH");
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(HEADLIGHT_LEFT, OUTPUT);
  pinMode(HEADLIGHT_RIGHT, OUTPUT);
  pinMode(TAILLIGHT_LEFT, OUTPUT);
  pinMode(TAILLIGHT_RIGHT, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LEFT_IR, INPUT);
  pinMode(RIGHT_IR, INPUT);

  ledcAttach(ENA, 1000, 8);
  ledcAttach(ENB, 1000, 8);
}

// Main Loop
void loop() {
  if (SerialBT.available()) {
    char command = SerialBT.read();
    Serial.print("Received: ");
    Serial.println(command);

    switch (command) {
      case 'F': if (!isEBrakeActive && isPowerOn) moveForward(); break;
      case 'B': if (!isEBrakeActive && isPowerOn) moveBackward(); break;
      case 'L': if (!isEBrakeActive && isPowerOn) turnLeft(); break;
      case 'R': if (!isEBrakeActive && isPowerOn) turnRight(); break;
      case 'S': stopMotors(); break;
      case 'H': headlightsOn(); break;
      case 'h': headlightsOff(); break;
      case 'T': taillightsOn(); break;
      case 't': taillightsOff(); break;
      case 'I': leftIndicatorOn(); break;
      case 'i': leftIndicatorOff(); break;
      case 'J': rightIndicatorOn(); break;
      case 'j': rightIndicatorOff(); break;
      case 'Z': digitalWrite(BUZZER, HIGH); break;
      case 'X': digitalWrite(BUZZER, LOW); break;
      case 'D': danceMode_MissionImpossible(); break;
      case 'M': musicMode(); break;
      case 'K': driftMode(); break;
      case '3': spin360(); break;
      case 'O': obstacleAvoidanceMode(); break;
      case 'Q': lockCarMode(); break;
      case 'U': disableLockMode(); break;
      case 'G': activateEBrake(); break;
      case 'N': deactivateEBrake(); break;
      case 'P': powerOnSequence(); break;
      case 'W': powerOffSequence(); break;
      case '7': setPowerMode('N'); break;
      case 'E': setPowerMode('E'); break;
      case '4': setPowerMode('V'); break;
      case '9': dynamicMorseMode(); break;
    }
  }

  // Handle Indicators
  if (leftIndicatorActive || rightIndicatorActive) {
    unsigned long currentMillis = millis();
    if (currentMillis - indicatorPreviousMillis >= indicatorInterval) {
      indicatorPreviousMillis = currentMillis;
      indicatorState = !indicatorState;
      if(leftIndicatorActive) {
        digitalWrite(HEADLIGHT_LEFT, indicatorState);
        digitalWrite(TAILLIGHT_LEFT, indicatorState);
        digitalWrite(BUZZER, indicatorState);
      }
      if(rightIndicatorActive) {
        digitalWrite(HEADLIGHT_RIGHT, indicatorState);
        digitalWrite(TAILLIGHT_RIGHT, indicatorState);
        digitalWrite(BUZZER, indicatorState);
      }
    }
  }
}

// --- OPTIMIZATION --- New central function to ramp motor speed and prevent brownouts
void setMotorSpeed(int leftSpeed, int rightSpeed) {
  // Ramp up/down the left motor speed
  if (leftSpeed > currentLeftSpeed) {
    for (int i = currentLeftSpeed; i <= leftSpeed; i += 15) {
      ledcWrite(ENA, i);
      delay(5);
    }
  } else {
    for (int i = currentLeftSpeed; i >= leftSpeed; i -= 15) {
      ledcWrite(ENA, i);
      delay(5);
    }
  }
  currentLeftSpeed = leftSpeed;

  // Ramp up/down the right motor speed
  if (rightSpeed > currentRightSpeed) {
    for (int i = currentRightSpeed; i <= rightSpeed; i += 15) {
      ledcWrite(ENB, i);
      delay(5);
    }
  } else {
    for (int i = currentRightSpeed; i >= rightSpeed; i -= 15) {
      ledcWrite(ENB, i);
      delay(5);
    }
  }
  currentRightSpeed = rightSpeed;
}

// --- All movement functions now use the optimized setMotorSpeed() function ---
void moveForward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setMotorSpeed(speedA, speedB);
}

void moveBackward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  setMotorSpeed(speedA, speedB);
}

void turnLeft() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  setMotorSpeed(speedA, speedB);
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setMotorSpeed(speedA, speedB);
}

void stopMotors() {
  setMotorSpeed(0, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// Lights
void headlightsOn() {
  digitalWrite(HEADLIGHT_LEFT, HIGH);
  digitalWrite(HEADLIGHT_RIGHT, HIGH);
}
void headlightsOff() {
  digitalWrite(HEADLIGHT_LEFT, LOW);
  digitalWrite(HEADLIGHT_RIGHT, LOW);
}
void taillightsOn() {
  digitalWrite(TAILLIGHT_LEFT, HIGH);
  digitalWrite(TAILLIGHT_RIGHT, HIGH);
}
void taillightsOff() {
  digitalWrite(TAILLIGHT_LEFT, LOW);
  digitalWrite(TAILLIGHT_RIGHT, LOW);
}
void allLightsOn() {
  headlightsOn();
  taillightsOn();
}
void leftIndicatorOn() {
  leftIndicatorActive = true;
  rightIndicatorActive = false;
}
void leftIndicatorOff() {
  leftIndicatorActive = false;
  digitalWrite(HEADLIGHT_LEFT, LOW);
  digitalWrite(TAILLIGHT_LEFT, LOW);
  digitalWrite(BUZZER, LOW);
}
void rightIndicatorOn() {
  rightIndicatorActive = true;
  leftIndicatorActive = false;
}
void rightIndicatorOff() {
  rightIndicatorActive = false;
  digitalWrite(HEADLIGHT_RIGHT, LOW);
  digitalWrite(TAILLIGHT_RIGHT, LOW);
  digitalWrite(BUZZER, LOW);
}
void allLightsOff() {
  headlightsOff();
  taillightsOff();
}

// All sound sequences now use simple digitalWrite beeps
void powerOnSequence() {
  digitalWrite(BUZZER, HIGH); delay(80); digitalWrite(BUZZER, LOW); delay(80);
  digitalWrite(BUZZER, HIGH); delay(80); digitalWrite(BUZZER, LOW); delay(80);
  digitalWrite(BUZZER, HIGH); delay(150); digitalWrite(BUZZER, LOW);

  headlightsOn();
  delay(150);
  taillightsOn();
  delay(300);
  allLightsOff();
  delay(100);
  allLightsOn();
  delay(150);
  allLightsOff();
  isPowerOn = true;
}

void powerOffSequence() {
  isPowerOn = false;
  stopMotors();
  taillightsOn(); delay(200); taillightsOff(); delay(200);
  taillightsOn(); delay(200); taillightsOff(); delay(400);

  digitalWrite(BUZZER, HIGH); delay(150); digitalWrite(BUZZER, LOW); delay(100);
  digitalWrite(BUZZER, HIGH); delay(100); digitalWrite(BUZZER, LOW);
  
  allLightsOff();
}

// Dance Mode
void danceMode_MissionImpossible() {
  while (SerialBT.available() == 0) {
    digitalWrite(BUZZER, HIGH); headlightsOn(); moveForward(); delay(300);
    digitalWrite(BUZZER, LOW); headlightsOff(); stopMotors(); delay(100);
    digitalWrite(BUZZER, HIGH); taillightsOn(); moveBackward(); delay(200);
    digitalWrite(BUZZER, LOW); taillightsOff(); stopMotors(); delay(100);
    digitalWrite(BUZZER, HIGH); turnLeft(); delay(400);
    digitalWrite(BUZZER, LOW); stopMotors(); delay(150);
    digitalWrite(BUZZER, HIGH); turnRight(); delay(400);
    digitalWrite(BUZZER, LOW); stopMotors(); delay(150);
    digitalWrite(BUZZER, HIGH);
    digitalWrite(HEADLIGHT_LEFT, HIGH);
    digitalWrite(TAILLIGHT_RIGHT, HIGH);
    moveForward(); delay(300);
    digitalWrite(BUZZER, LOW); allLightsOff(); stopMotors(); delay(300);
  }
  stopMotors();
}

// Music mode with synchronized lights
void musicMode() {
  while (!SerialBT.available()) {
    for(int i=0; i<2; i++) {
      if(SerialBT.available()) break;
      digitalWrite(BUZZER, HIGH); allLightsOn(); delay(100); 
      digitalWrite(BUZZER, LOW); allLightsOff(); delay(100);
      digitalWrite(BUZZER, HIGH); allLightsOn(); delay(100); 
      digitalWrite(BUZZER, LOW); allLightsOff(); delay(100);
      digitalWrite(BUZZER, HIGH); allLightsOn(); delay(250); 
      digitalWrite(BUZZER, LOW); allLightsOff(); delay(250);
    }
    if(SerialBT.available()) break;
    delay(500);
  }
  allLightsOff();
}

// Drift Mode
void driftMode() {
  setPowerMode('V'); 
  while (!SerialBT.available()) {
    moveForward();
    delay(300);
    if (SerialBT.available()) break;

    turnRight();
    delay(250);
    if (SerialBT.available()) break;

    turnLeft();
    delay(250);
    if (SerialBT.available()) break;
  }
  stopMotors();
  setPowerMode('N'); 
}

// 360 Mode
void spin360() {
  turnLeft(); 
  unsigned long startTime = millis();
  while (millis() - startTime < 1200) {
      if (SerialBT.available()) break;
      delay(10);
  }
  stopMotors();
}


// Morse Mode
String getMorse(char c);
void buzzAndLight(int duration) {
  allLightsOn();
  digitalWrite(BUZZER, HIGH);
  delay(duration);
  digitalWrite(BUZZER, LOW);
  allLightsOff();
}
void playMorseChar(char c) {
  String morse = getMorse(c);
  for (int i = 0; i < morse.length(); i++) {
    if (SerialBT.available()) return;
    if (morse[i] == '.') { buzzAndLight(200); }
    else if (morse[i] == '-') { buzzAndLight(600); }
    delay(200);
  }
}
void dynamicMorseMode() {
  String message = "";
  unsigned long messageStartTime = millis();
  while (millis() - messageStartTime < 5000) {
    if (SerialBT.available()) {
      message += (char)SerialBT.read();
    }
  }
  if (message.length() == 0) return;
  
  message.toUpperCase();

  while (!SerialBT.available()) {
    for (int i = 0; i < message.length(); i++) {
      if (SerialBT.available()) break;
      char c = message[i];
      if (c == ' ') {
        delay(600);
      } else {
        playMorseChar(c);
        delay(600);
      }
    }
    if (SerialBT.available()) break;
    delay(3000);
  }
}
// --- OPTIMIZATION --- Trimmed getMorse to save space
String getMorse(char c) {
  switch (c) {
    case 'A': return ".-";  case 'B': return "-..."; case 'C': return "-.-.";
    case 'D': return "-.."; case 'E': return ".";    case 'F': return "..-.";
    case 'G': return "--."; case 'H': return "...."; case 'I': return "..";
    case 'J': return ".---";case 'K': return "-.-";  case 'L': return ".-..";
    case 'M': return "--";  case 'N': return "-.";   case 'O': return "---";
    case 'P': return ".--.";case 'Q': return "--.-"; case 'R': return ".-.";
    case 'S': return "..."; case 'T': return "-";    case 'U': return "..-";
    case 'V': return "...-";case 'W': return ".--";  case 'X': return "-..-";
    case 'Y': return "-.--";case 'Z': return "--..";
    case '0': return "-----"; case '1': return ".----"; case '2': return "..---";
    case '3': return "...--"; case '4': return "....-"; case '5': return ".....";
    case '6': return "-...."; case '7': return "--..."; case '8': return "---..";
    case '9': return "----.";
    default: return "";
  }
}


// --- Sensor and Lock Functions ---
long measureDistance() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 25000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

long getStableDistance() {
  long readings[5];
  for (int i = 0; i < 5; i++) {
    readings[i] = measureDistance();
    delay(10);
  }
  long sum = 0;
  int validReadings = 0;
  for (int i = 0; i < 5; i++) {
    if (readings[i] < 999) {
      sum += readings[i];
      validReadings++;
    }
  }
  if (validReadings == 0) return 999;
  return sum / validReadings;
}

int getStableIRReading(int pin) {
  int count = 0;
  for (int i = 0; i < 5; i++) {
    count += digitalRead(pin);
    delayMicroseconds(500);
  }
  return (count > 2) ? HIGH : LOW;
}

bool isLockModeActive = false;
bool alarmTriggered = false;

void lockingConfirmationSequence() {
  allLightsOn(); delay(250);
  allLightsOff(); delay(150);
  digitalWrite(BUZZER, HIGH); delay(250); digitalWrite(BUZZER, LOW);
}

void triggerAlarm() {
  digitalWrite(HEADLIGHT_LEFT, HIGH); digitalWrite(TAILLIGHT_LEFT, HIGH);
  digitalWrite(HEADLIGHT_RIGHT, LOW); digitalWrite(TAILLIGHT_RIGHT, LOW);
  digitalWrite(BUZZER, HIGH);
  delay(150);
  digitalWrite(HEADLIGHT_LEFT, LOW); digitalWrite(TAILLIGHT_LEFT, LOW);
  digitalWrite(HEADLIGHT_RIGHT, HIGH); digitalWrite(TAILLIGHT_RIGHT, HIGH);
  digitalWrite(BUZZER, LOW);
  delay(150);
}

void stopAlarm() {
  digitalWrite(BUZZER, LOW);
  allLightsOff();
  alarmTriggered = false;
}

void lockCarMode() {
  isLockModeActive = true;
  alarmTriggered = false;
  
  Serial.println("Arming alarm... getting stable sensor readings.");
  long initialDistance = getStableDistance();
  int initialLeftIR = getStableIRReading(LEFT_IR);
  int initialRightIR = getStableIRReading(RIGHT_IR);
  Serial.print("Armed with initial distance: "); Serial.println(initialDistance);
  Serial.print("Initial IR L/R: "); Serial.print(initialLeftIR); Serial.print("/"); Serial.println(initialRightIR);
  
  lockingConfirmationSequence();

  while (isLockModeActive) {
    if (!alarmTriggered) {
      long currentDistance = getStableDistance();
      int currentLeftIR = getStableIRReading(LEFT_IR);
      int currentRightIR = getStableIRReading(RIGHT_IR);

      if (abs(currentDistance - initialDistance) > 15 || currentLeftIR != initialLeftIR || currentRightIR != initialRightIR) {
        Serial.println("Intrusion detected!");
        alarmTriggered = true;
      }
    }
    if (alarmTriggered) {
      triggerAlarm();
    }
    if (SerialBT.available()) {
      char cmd = SerialBT.read();
      if (cmd == 'U') {
        disableLockMode();
        break;
      }
    }
    if (!alarmTriggered) delay(100);
  }
}

void disableLockMode() {
  isLockModeActive = false;
  stopAlarm();
  allLightsOn(); delay(100); allLightsOff(); delay(100);
  allLightsOn(); delay(100); allLightsOff();
  digitalWrite(BUZZER, HIGH); delay(80); digitalWrite(BUZZER, LOW); delay(100);
  digitalWrite(BUZZER, HIGH); delay(80); digitalWrite(BUZZER, LOW);
}

// Obstacle Avoidance Mode
bool isObstacleModeActive = false;
void alertByDistance(long distance) {
  int delayTime;
  if (distance > 100) return;
  else if (distance > 70) delayTime = 500;
  else if (distance > 50) delayTime = 300;
  else if (distance > 30) delayTime = 200;
  else delayTime = 100;
  allLightsOn();
  digitalWrite(BUZZER, HIGH);
  delay(delayTime);
  digitalWrite(BUZZER, LOW);
  allLightsOff();
  delay(delayTime);
}
void moveForwardSlow() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(ENA, 150); ledcWrite(ENB, 150);
  currentLeftSpeed = 150; currentRightSpeed = 150;
}
void obstacleAvoidanceMode() {
  isObstacleModeActive = true;
  while (isObstacleModeActive) {
    long distance = measureDistance();
    if (distance < 100) { alertByDistance(distance); }
    if (distance < 20) {
      stopMotors(); moveBackward(); delay(500);
      turnLeft(); delay(800); stopMotors(); delay(100);
    } else if (distance < 50) {
      moveForwardSlow();
    } else {
      moveForward();
    }
    if (SerialBT.available()) {
      stopMotors(); isObstacleModeActive = false; break;
    }
    delay(50);
  }
  stopMotors();
}

// E Brake with Brake Lights
void activateEBrake() {
  isEBrakeActive = true;
  stopMotors();
  taillightsOn();
  Serial.println("E-BRAKE ACTIVATED! Car is locked.");
}
void deactivateEBrake() {
  isEBrakeActive = false;
  taillightsOff();
  Serial.println("E-BRAKE DEACTIVATED! Car is free to move.");
}

// Drive Modes
void setPowerMode(char mode) {
  switch (mode) {
    case 'N': speedA = 160; speedB = 160; break;
    case 'E': speedA = 130; speedB = 130; break;
    case 'V': 
      speedA = 245; 
      speedB = 245; 
      break;
    default: break;
  }
}
