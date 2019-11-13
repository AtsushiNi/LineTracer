int sensor1Pin = A0;
int sensor2Pin = A1;
void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print("light1:");
  Serial.print(analogRead(sensor1Pin));
  Serial.print(",light2:");
  Serial.println(analogRead(sensor2Pin));
}
