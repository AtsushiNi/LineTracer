// AndroidとのBluetooth検証用コード

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    int input = Serial.read();
    Serial.print(input);
    Serial.print((char)input); 
  }
}
