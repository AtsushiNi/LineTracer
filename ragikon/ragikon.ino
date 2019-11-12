# pin
int redLedPin = 3;
int greenLedPin = 5;
int blueLedPin = 6;

int rightMoterPin = 4;
int leftMoterPin = 11;

# 速度
int speed = 100;
float racio = 0.6;

void setup() {
  Serial.begin(9600);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(rightMoterPin, OUTPUT);
  pinMode(leftMoterPin, OUTPUT);
  analogWrite(rightMoterPin, speed);
  analogWrite(leftMoterPin, speed);
}

void loop() {
  digitalWrite(blueLedPin, LOW);
  if (Serial.available()>0) {
    digitalWrite(blueLedPin, HIGH);
    int input = Serial.read();
    Serial.println(input);

    if (input==48) { # code = 0 直進
      # LED
      digitalWrite(greenLedPin, HIGH);
      digitalWrite(redLedPin, HIGH);
      # MOTER
      analogWrite(rightMoterPin, speed);
      analogWrite(leftMoterPin, speed);

    } else if(input==49) { # code = 1 左折
      # LED
      digitalWrite(greenLedPin, LOW);
      digitalWrite(redLedPin, HIGH);
      # MOTER
      analogWrite(rightMoterPin, speed);
      analogWrite(leftMoterPin, speed * racio);

    } else if(input == 50) { # code = 2 右折
      # LED
      digitalWrite(redLedPin, LOW);
      digitalWrite(greenLedPin, HIGH);
      # MOTER
      analogWrite(rightMoterPin, speed * racio);
      analogWrite(leftMoterPin, speed);
    }
  }
}
