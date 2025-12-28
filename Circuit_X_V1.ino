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

//IR Sensor Pins

#define LEFT_IR 34
#define RIGHT_IR 35

// Light & Buzzer Pins

#define HEADLIGHT_LEFT 15
#define HEADLIGHT_RIGHT 2
#define TAILLIGHT_LEFT 18
#define TAILLIGHT_RIGHT 5
#define BUZZER 4

// PWM Channels

#define ENA_CH 0
#define ENB_CH 1

// Notes definitions

#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978

//Bool Definitions

bool isPowerOn = false;
bool leftIndicatorActive = false;
bool rightIndicatorActive = false;
unsigned long indicatorPreviousMillis = 0;
const long indicatorInterval = 400;
bool indicatorState = false;
bool isEBrakeActive = false;
bool isCombinedModeActive = false;
int speedA = 160;
int speedB = 160;

//Setup Loop
void setup() {
  Serial.begin(115200);
  SerialBT.begin("Circuit X");
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
  ledcSetup(ENA_CH, 1000, 8);
  ledcSetup(ENB_CH, 1000, 8);
  ledcAttachPin(ENA, ENA_CH);
  ledcAttachPin(ENB, ENB_CH);
}

//Main Loop

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
      case 'Y': irLine(); break;
      case 'O': obstacleAvoidanceMode();break;
      case '5': combinedMode(); break;
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


// Handle Left Indicator Blinking
  if (leftIndicatorActive) {
    unsigned long currentMillis = millis();
    if (currentMillis - indicatorPreviousMillis >= indicatorInterval) {
      indicatorPreviousMillis = currentMillis;
      indicatorState = !indicatorState;
      digitalWrite(HEADLIGHT_LEFT, indicatorState);
      digitalWrite(TAILLIGHT_LEFT, indicatorState);
      digitalWrite(BUZZER, indicatorState);
    }
  }

// Handle Right Indicator Blinking
  if (rightIndicatorActive) {
    unsigned long currentMillis = millis();
    if (currentMillis - indicatorPreviousMillis >= indicatorInterval) {
      indicatorPreviousMillis = currentMillis;
      indicatorState = !indicatorState;
      digitalWrite(HEADLIGHT_RIGHT, indicatorState);
      digitalWrite(TAILLIGHT_RIGHT, indicatorState);
      digitalWrite(BUZZER, indicatorState);
    }
  }
}


//Movement Functions

void moveForward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
}

void moveBackward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA_CH, speedA);
  analogWrite(ENB_CH, speedB);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA_CH, speedA);
  analogWrite(ENB_CH, speedB);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA_CH, 0);
  analogWrite(ENB_CH, 0);
}

//Lights

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
  digitalWrite(HEADLIGHT_LEFT, LOW);
  digitalWrite(HEADLIGHT_RIGHT, LOW);
  digitalWrite(TAILLIGHT_LEFT, LOW);
  digitalWrite(TAILLIGHT_RIGHT, LOW);
}

//Main Functions

//Power On Sequence

void powerOnSequence() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(HEADLIGHT_LEFT, HIGH);
    digitalWrite(HEADLIGHT_RIGHT, HIGH);
    digitalWrite(TAILLIGHT_LEFT, HIGH);
    digitalWrite(TAILLIGHT_RIGHT, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(300);

    digitalWrite(HEADLIGHT_LEFT, LOW);
    digitalWrite(HEADLIGHT_RIGHT, LOW);
    digitalWrite(TAILLIGHT_LEFT, LOW);
    digitalWrite(TAILLIGHT_RIGHT, LOW);
    digitalWrite(BUZZER, LOW);
    delay(300);
  }

  moveForward();
  delay(800);
  stopMotors();
  delay(300);

  moveBackward();
  delay(800);
  stopMotors();
  delay(300);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA_CH, 200);
  analogWrite(ENB_CH, 200);
  delay(3300);
  stopMotors();
  delay(300);

  for (int i = 0; i < 2; i++) {
    digitalWrite(HEADLIGHT_LEFT, HIGH);
    digitalWrite(HEADLIGHT_RIGHT, HIGH);
    digitalWrite(TAILLIGHT_LEFT, HIGH);
    digitalWrite(TAILLIGHT_RIGHT, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(400);

    digitalWrite(HEADLIGHT_LEFT, LOW);
    digitalWrite(HEADLIGHT_RIGHT, LOW);
    digitalWrite(TAILLIGHT_LEFT, LOW);
    digitalWrite(TAILLIGHT_RIGHT, LOW);
    digitalWrite(BUZZER, LOW);
    delay(400);
  }

  digitalWrite(HEADLIGHT_LEFT, LOW);
  digitalWrite(HEADLIGHT_RIGHT, LOW);
  digitalWrite(TAILLIGHT_LEFT, LOW);
  digitalWrite(TAILLIGHT_RIGHT, LOW);
  digitalWrite(BUZZER, LOW);
  isPowerOn = true;
}

void powerOffSequence() {
  isPowerOn = false;
}

// Dance Mode

void danceMode_MissionImpossible() {
  while (SerialBT.available() == 0) {
    // Beat 1
    digitalWrite(BUZZER, HIGH);
    digitalWrite(HEADLIGHT_LEFT, HIGH);
    digitalWrite(HEADLIGHT_RIGHT, HIGH);
    moveForward();
    delay(300);

    // Beat 2
    digitalWrite(BUZZER, LOW);
    digitalWrite(HEADLIGHT_LEFT, LOW);
    digitalWrite(HEADLIGHT_RIGHT, LOW);
    stopMotors();
    delay(100);

    digitalWrite(BUZZER, HIGH);
    digitalWrite(TAILLIGHT_LEFT, HIGH);
    digitalWrite(TAILLIGHT_RIGHT, HIGH);
    moveBackward();
    delay(200);

    // Beat 3
    digitalWrite(BUZZER, LOW);
    digitalWrite(TAILLIGHT_LEFT, LOW);
    digitalWrite(TAILLIGHT_RIGHT, LOW);
    stopMotors();
    delay(100);

    digitalWrite(BUZZER, HIGH);
    turnLeft();
    delay(400);

    // Beat 4
    digitalWrite(BUZZER, LOW);
    stopMotors();
    delay(150);

    digitalWrite(BUZZER, HIGH);
    turnRight();
    delay(400);

    // Beat 5
    digitalWrite(BUZZER, LOW);
    stopMotors();
    delay(150);

    digitalWrite(BUZZER, HIGH);
    digitalWrite(HEADLIGHT_LEFT, HIGH);
    digitalWrite(TAILLIGHT_RIGHT, HIGH);
    moveForward();
    delay(300);

    // Pause
    digitalWrite(BUZZER, LOW);
    allLightsOff();
    stopMotors();
    delay(300);
  }
  stopMotors();
  digitalWrite(BUZZER, LOW);
  allLightsOff();
}

// Music Mode

int coffinMelody[] = {
  NOTE_E5, NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_G4,
  NOTE_C5, NOTE_G4, NOTE_E4, NOTE_A4, NOTE_B4, NOTE_AS4, NOTE_A4
};

int coffinDurations[] = {
  4, 4, 4, 4, 4, 2, 2,
  4, 4, 4, 4, 4, 4, 2
};

void playCoffinNote(int note, int duration) {
  int noteDuration = 1000 / duration;
  tone(BUZZER, note, noteDuration);
  for (int i = 0; i < noteDuration; i += 100) {
    digitalWrite(HEADLIGHT_LEFT, random(0, 2));
    digitalWrite(HEADLIGHT_RIGHT, random(0, 2));
    digitalWrite(TAILLIGHT_LEFT, random(0, 2));
    digitalWrite(TAILLIGHT_RIGHT, random(0, 2));
    delay(100);
  }
  noTone(BUZZER);
  delay(50);
}

void musicMode() {
  while (!SerialBT.available()) {
    for (int i = 0; i < sizeof(coffinMelody) / sizeof(coffinMelody[0]); i++) {
      playCoffinNote(coffinMelody[i], coffinDurations[i]);
      if (SerialBT.available()) return;
    }
  }

  digitalWrite(HEADLIGHT_LEFT, LOW);
  digitalWrite(HEADLIGHT_RIGHT, LOW);
  digitalWrite(TAILLIGHT_LEFT, LOW);
  digitalWrite(TAILLIGHT_RIGHT, LOW);
  noTone(BUZZER);
}

//Drift Mode

void driftLightShow() {
  digitalWrite(HEADLIGHT_LEFT, HIGH);
  digitalWrite(HEADLIGHT_RIGHT, LOW);
  digitalWrite(TAILLIGHT_LEFT, HIGH);
  digitalWrite(TAILLIGHT_RIGHT, LOW);
  delay(50);

  digitalWrite(HEADLIGHT_LEFT, LOW);
  digitalWrite(HEADLIGHT_RIGHT, HIGH);
  digitalWrite(TAILLIGHT_LEFT, LOW);
  digitalWrite(TAILLIGHT_RIGHT, HIGH);
  delay(50);
}

void driftMode() {
  while (!SerialBT.available()) {
  
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 255);
    analogWrite(ENB, 255);
    driftLightShow();
    delay(300);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 255);
    analogWrite(ENB, 255);
    driftLightShow();
    delay(250);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, 255);
    analogWrite(ENB, 255);
    driftLightShow();
    delay(250);

    if (SerialBT.available()) break;
  }
    stopMotors();
    allLightsOff();
}

// 360 Mode

void spin360() {
  while (!SerialBT.available()) {
  digitalWrite(IN1, LOW); 
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
  delay(1200);
  stopMotors();
  if (SerialBT.available()) break;}
}

//Morse Mode

void dynamicMorseMode() {
  String message = "";
  while (SerialBT.available() == 0) {
    delay(10);
  }

  while (SerialBT.available()) {
    char c = SerialBT.read();
    message += c;
    delay(5);
  }

  message.toUpperCase();

  while (true) {
    if (SerialBT.available()) {
      char command = SerialBT.read();
      if (command != '9') return;
    }

    for (int i = 0; i < message.length(); i++) {
      if (SerialBT.available()) {
        char command = SerialBT.read();
        if (command != '9') return;
      }

      char c = message[i];

      if (c == ' ') {
        delay(600);
      } else if (c == '-') {
        delay(300);
      } else {
        playMorseChar(c);
        delay(600);
      }
    }

    delay(3000);
  }
}
void playMorseChar(char c) {
  String morse = getMorse(c);
  for (int i = 0; i < morse.length(); i++) {
    if (morse[i] == '.') {
      buzzAndLight(200);
    } else if (morse[i] == '-') {
      buzzAndLight(600);
    }
    delay(200);
  }
}

void buzzAndLight(int duration) {
  digitalWrite(HEADLIGHT_LEFT, HIGH);
  digitalWrite(HEADLIGHT_LEFT, HIGH);
  digitalWrite(TAILLIGHT_LEFT, HIGH);
  digitalWrite(TAILLIGHT_RIGHT, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(duration);
  digitalWrite(BUZZER, LOW);
  digitalWrite(HEADLIGHT_RIGHT, LOW);
  digitalWrite(HEADLIGHT_RIGHT, LOW);
  digitalWrite(TAILLIGHT_LEFT, LOW);
  digitalWrite(TAILLIGHT_RIGHT, LOW);
}

String getMorse(char c) {
  switch (c) {
    // Letters A–Z
    case 'A': return ".-";
    case 'B': return "-...";
    case 'C': return "-.-.";
    case 'D': return "-..";
    case 'E': return ".";
    case 'F': return "..-.";
    case 'G': return "--.";
    case 'H': return "....";
    case 'I': return "..";
    case 'J': return ".---";
    case 'K': return "-.-";
    case 'L': return ".-..";
    case 'M': return "--";
    case 'N': return "-.";
    case 'O': return "---";
    case 'P': return ".--.";
    case 'Q': return "--.-";
    case 'R': return ".-.";
    case 'S': return "...";
    case 'T': return "-";
    case 'U': return "..-";
    case 'V': return "...-";
    case 'W': return ".--";
    case 'X': return "-..-";
    case 'Y': return "-.--";
    case 'Z': return "--..";

    // Numbers 0–9
    case '0': return "-----";
    case '1': return ".----";
    case '2': return "..---";
    case '3': return "...--";
    case '4': return "....-";
    case '5': return ".....";
    case '6': return "-....";
    case '7': return "--...";
    case '8': return "---..";
    case '9': return "----.";

    // Symbols
    case '.': return ".-.-.-";
    case ',': return "--..--";
    case '?': return "..--..";
    case '!': return "-.-.--";
    case ':': return "---...";
    case '\'': return ".----.";
    case '"': return ".-..-.";
    case '/': return "-..-.";
    case '(': return "-.--.";
    case ')': return "-.--.-";
    case '&': return ".-...";
    case '=': return "-...-";
    case '+': return ".-.-.";
    case '-': return "-....-";
    case '_': return "..--.-";
    case ';': return "-.-.-.";
    case '@': return ".--.-.";
    case '$': return "...-..-";

    default: return "";
  }
}

// IR Mode

void moveForwardir() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 120); 
  analogWrite(ENB, 120);
}

void turnLeftir() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 200);
  analogWrite(ENB, 0);
}

void turnRightir() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA,0);
  analogWrite(ENB,200); 
}

int getStableIRReading(int pin) {
  int count = 0;
  for (int i = 0; i < 5; i++) {
    count += digitalRead(pin);
    delayMicroseconds(500);
  }
  return (count > 2) ? HIGH : LOW;
}

void irLine() {
  
  while (!SerialBT.available()) {

    int leftIR = getStableIRReading(LEFT_IR);
    int rightIR = getStableIRReading(RIGHT_IR);

    if (leftIR == HIGH && rightIR == HIGH) {
      moveForwardir();
    } 
    else if (leftIR == LOW && rightIR == HIGH) {
      delay(5);
      if (getStableIRReading(LEFT_IR) == LOW && getStableIRReading(RIGHT_IR) == HIGH) {
        turnRightir();
      }
    } 
    else if (leftIR == HIGH && rightIR == LOW) {
      delay(5);
      if (getStableIRReading(LEFT_IR) == HIGH && getStableIRReading(RIGHT_IR) == LOW) {
        turnLeftir();
      }
    } 
    else {
      stopMotors();
    }
    delay(10);
  }
}

// Ultrasonic Sensor Mode

long measureDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

// Lock Mode

bool isLockModeActive = false;
bool alarmTriggered = false;

void lockConfirmationSequence() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER, HIGH);
    headlightsOn();
    taillightsOn();
    delay(200);
    digitalWrite(BUZZER, LOW);
    headlightsOff();
    taillightsOff();
    delay(200);
  }
}

void triggerAlarm() {
  digitalWrite(BUZZER, HIGH);
  headlightsOn();
  taillightsOn();
  delay(200);
  digitalWrite(BUZZER, LOW);
  headlightsOff();
  taillightsOff();
  delay(200);
}

void stopAlarm() {
  digitalWrite(BUZZER, LOW);
  headlightsOff();
  taillightsOff();
  leftIndicatorOff();
  rightIndicatorOff();
  alarmTriggered = false;
}

void lockCarMode() {
  isLockModeActive = true;
  alarmTriggered = false;

  int initialLeftIR = digitalRead(LEFT_IR);
  int initialRightIR = digitalRead(RIGHT_IR);
  long initialDistance = measureDistance();

  lockConfirmationSequence();

  while (isLockModeActive) {
    if (!alarmTriggered) {
      int currentLeftIR = digitalRead(LEFT_IR);
      int currentRightIR = digitalRead(RIGHT_IR);
      long currentDistance = measureDistance();

      if (currentLeftIR != initialLeftIR || currentRightIR != initialRightIR || abs(currentDistance - initialDistance) > 15) {
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
      }
    }
  }
}

void disableLockMode() {
  isLockModeActive = false;
  stopAlarm();
}

// Obstacle Avoidance Mode

bool isObstacleModeActive = false;

void alertByDistance(long distance) {
  int delayTime;

  if (distance > 100) {
    return;
  } else if (distance > 70) {
    delayTime = 500;
  } else if (distance > 50) {
    delayTime = 300;
  } else if (distance > 30) {
    delayTime = 200;
  } else {
    delayTime = 100;
  }

  digitalWrite(HEADLIGHT_LEFT, HIGH);
  digitalWrite(HEADLIGHT_RIGHT, HIGH);
  digitalWrite(TAILLIGHT_LEFT, HIGH);
  digitalWrite(TAILLIGHT_RIGHT, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(delayTime);
  digitalWrite(BUZZER, LOW);
  digitalWrite(HEADLIGHT_RIGHT, LOW);
  digitalWrite(HEADLIGHT_LEFT, LOW);
  digitalWrite(TAILLIGHT_RIGHT, LOW);
  digitalWrite(TAILLIGHT_LEFT, LOW);
  delay(delayTime);
}


void moveForwardSlow() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}


void avoidObstacle() {
  stopMotors();
  alertByDistance(0);
  
  int turnTime = random(1000, 2000);
  if (random(0, 2) == 0) {
    turnLeft();
  } else {
    turnRight();
  }
  delay(turnTime);
  stopMotors();
}

void obstacleAvoidanceMode() {
  isObstacleModeActive = true;

  while (isObstacleModeActive) {
    long distance = measureDistance();

    if (distance < 100) {
      alertByDistance(distance);
    }

    if (distance < 20) {
      stopMotors();
      alertByDistance(0);
      turnLeft();
      delay(800);
      stopMotors();
      delay(100);

      long newDistance = measureDistance();

      if (newDistance < 20) {
        turnRight();
        delay(1000);
        stopMotors();
        delay(100);
      }
    } 
    else if (distance < 50) {
      moveForwardSlow();
    } 
    else {
      moveForward();
    }

    if (SerialBT.available()) {
      char cmd = SerialBT.read();
      stopMotors();
      isObstacleModeActive = false;
      break;
    }
    delay(50);
  }
}

// E Brake

void activateEBrake() {
  isEBrakeActive = true;
  stopMotors();
  Serial.println("E-BRAKE ACTIVATED! Car is locked.");
}

void deactivateEBrake() {
  isEBrakeActive = false;
  Serial.println("E-BRAKE DEACTIVATED! Car is free to move.");
}

// Combined Mode

void combinedMode() {
  isCombinedModeActive = true;
  while (isCombinedModeActive) {
    if (SerialBT.available()) {
      char cmd = SerialBT.read();
      stopMotors();
      isCombinedModeActive = false;
      break;
    }

    long distance = measureDistance();

    if (distance < 15) {
      stopMotors();
      Serial.println("Obstacle on path. Stopping...");
      continue;
    }
    int leftIR = getStableIRReading(LEFT_IR);
    int rightIR = getStableIRReading(RIGHT_IR);

    if (leftIR == HIGH && rightIR == HIGH) {
      moveForwardir();
    } 
    else if (leftIR == LOW && rightIR == HIGH) {
      delay(5);
      if (getStableIRReading(LEFT_IR) == LOW && getStableIRReading(RIGHT_IR) == HIGH) {
        turnRightir();
      }
    } 
    else if (leftIR == HIGH && rightIR == LOW) {
      delay(5);
      if (getStableIRReading(LEFT_IR) == HIGH && getStableIRReading(RIGHT_IR) == LOW) {
        turnLeftir();
      }
    } 
    else {
      stopMotors();
    }
    delay(10);
    
  }
}

// Drive Modes

void setPowerMode(char mode) {
  switch (mode) {
    case 'N':
      speedA = 160;
      speedB = 160;
      break;

    case 'E':
      speedA = 130;
      speedB = 130;
      break;

    case 'V':
      speedA = 255;
      speedB = 255;
      break;

    default:
      break;
  }
}
