// Pin
int rightMoterPin = 11;
int leftMoterPin = 3;
int rightSensorPin = A0;
int leftSensorPin = A1;

// Bluetooth入力用
char inputMode = 'a'; // 入力モード。aは入力待機
char inputValue[8]; // 入力中の値。モードがaになったら各パラメーターに代入
int inputPointer = 0; // 値入力中に使うポインタ用

// 入力で変更できるパラメータ
int basicStatus = 0; // 基本ステータス。0:待機, 1:走行
int runMode = 0; // 走行モード　0:ライントレース, 1:ラジコン
int radioControllDirection = 0; // ラジコンモードの進路方向　0:直進, 1:右, 2: 左
int basicSpeed = 250; // 走行スピード
float reduceRacio = 0.2; // ラジコンモードでの左右減速比
int outputMoterPower = 0; // モーター出力値を出力するかどうか 0:しない, 1:する
int outputLightSensor = 1; // 光センサーの値を出力するかどうか 0:しない, 1:する
int outputMode = 0; // 表示出力の様式　0:ArduinoIDE, 1:Android, 2:ターミナル

void setup() {
  Serial.begin(9600);
  pinMode(rightMoterPin, OUTPUT);
  pinMode(leftMoterPin, OUTPUT);
}

void loop() {
  // シリアル通信入力処理
  if (Serial.available() > 0) {
    char input = (char)Serial.read();

    if ('a' <= input && input <= 'z') { // コマンド入力
      if (input == 'a' && inputMode != 'a') { // モードがa以外からaに変わるとパラメータにinputValueが代入される
        char tmp[inputPointer];
        memcpy(tmp, inputValue, inputPointer);
        switch (inputMode) {
          case 'b':
            basicStatus = atoi(tmp);
          break;
          case 'c':
            runMode = atoi(tmp);
          break;
          case 'd':
            radioControllDirection = atoi(tmp);
          break;
          case 'e':
            basicSpeed = atoi(tmp);
          break;
          case 'f':
            reduceRacio = atof(tmp);
          break;
          case 'g':
            outputMoterPower = atoi(tmp);
          break;
          case 'h':
            outputMode = atoi(tmp);
          break;
          case 'i':
            outputLightSensor = atoi(tmp);
          break;
        }
        inputPointer = 0;
      }
      inputMode = input;

    } else if (('0' <= input && input <= '9') || input == '.') { // 値入力
      inputValue[inputPointer] = input;
      ++inputPointer;
    }
  }

  // 光センサー入力
  int rightData = analogRead(rightSensorPin);
  int leftData = analogRead(leftSensorPin);

  // 現在位置の算出
  float position = 0; //線上が0. 機体の右へのずれが正. 両端のセンサー位置が±1
  position = calcuPosBy2(rightData, leftData);

  // モーター出力計算
  float rightPower = 0; // 右モーター出力
  float leftPower = 0; // 左モーター出力

  if (runMode == 1) {
    rightPower = (float)basicSpeed;
    leftPower = (float)basicSpeed;
    switch (radioControllDirection) {
      case 1:
        rightPower *= reduceRacio;
      break;
      case 2:
        leftPower *= reduceRacio;
      break;
    }
  }

  // モーター出力
  if (basicStatus == 1) {
    analogWrite(rightMoterPin, rightPower);
    analogWrite(leftMoterPin, leftPower);
  } else {
    analogWrite(rightMoterPin, 0);
    analogWrite(leftMoterPin, 0);
  }

  // 表示出力
  if (outputMoterPower) {
    switch(outputMode) {
      case 0:
        Serial.print(leftPower);
        Serial.print(",");
        Serial.println(rightPower);
        break;
      case 1:
        Serial.print(rightPower);
        Serial.print('*');
        Serial.println(leftPower);
        Serial.println('***');
       break;
    }
  }
  if (outputLightSensor) {
    switch(outputMode) {
      case 0:
        Serial.print(leftData);
        Serial.print(',');
        Serial.println(rightData);
      break;
      case 1:
        Serial.print(rightData);
        Serial.print('*');
        Serial.print(leftData);
        Serial.print('***');
      break;
    }
  }
}
