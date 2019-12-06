  // Pin
int rightMoterPin = 11;
int leftMoterPin = 3;
int sensorPin1 = A0;
int sensorPin2 = A1;
int sensorPin3 = A2;
int sensorPin4 = A3;

// Bluetooth入力用
char inputMode = 'a'; // 入力モード。aは入力待機
char inputValue[8]; // 入力中の値。モードがaになったら各パラメーターに代入
int inputPointer = 0; // 値入力中に使うポインタ用

// 入力で変更できるパラメータ
int basicStatus = 0; // 基本ステータス。0:待機, 1:走行
int runMode = 0; // 走行モード　0:ライントレース, 1:ラジコン, 2:テスト
int radioControllDirection = 0; // ラジコンモードの進路方向　0:直進, 1:右, 2: 左
int basicSpeed = 100; // 走行スピード. アナログ出力の最大は255
float reduceRacio = 0.2; // ラジコンモードでの左右減速比
int outputMoterPower = 0; // モーター出力値を出力するかどうか 0:しない, 1:する
int outputLightSensor = 1; // 光センサーの値を出力するかどうか 0:しない, 1:する
int outputMode = 2; // 表示出力の様式　0:ArduinoIDE, 1:Android, 2:Python
float kp = 0; // 比例制御のパラメータ
float ki = 0; // 積分制御のパラメータ
float kd = 0; // 微分制御のパラメータ
// センサ値解析用近似式の係数。右センサー前半から、低次項から順
float polyfit_params_11[2];
float polyfit_params_12[2];
float polyfit_params_21[2];
float polyfit_params_22[2];
float polyfit_params_31[2];
float polyfit_params_32[2];
float polyfit_params_41[2];
float polyfit_params_42[2];
int sensor_borders[4]; // センサーの有効範囲(線を見失っているかどうかのボーダー値)。右センサーから。

// PID制御の変数
float pos_1 = 0; // 前回の位置
float pos_2 = 0; // 前々回の位置

// 線を見失った時のための変数
int direction_his = 1;

void setup() {
  Serial.begin(9600);
  pinMode(rightMoterPin, OUTPUT);
  pinMode(leftMoterPin, OUTPUT);
}

void loop() {
  // シリアル通信入力処理
  if (Serial.available() > 0) {
    char input = (char)Serial.read();

    if (('a' <= input && input <= 'z') || ('A' <= input && input <= 'Z')) { // コマンド入力
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
          case 'j':
            kp = atof(tmp);
            break;
          case 'k':
            ki = atof(tmp);
            break;
          case 'l':
            kd = atof(tmp);
            break;
          case 'A':
            polyfit_params_11[0] = atof(tmp);
            break;
          case 'B':
            polyfit_params_11[1] = atof(tmp);
            break;
          case 'C':
            polyfit_params_12[0] = atof(tmp);
            break;
          case 'D':
            polyfit_params_12[1] = atof(tmp);
            break;
          case 'E':
            polyfit_params_21[0] = atof(tmp);
            break;
          case 'F':
            polyfit_params_21[1] = atof(tmp);
            break;
          case 'G':
            polyfit_params_22[0] = atof(tmp);
            break;
          case 'H':
            polyfit_params_22[1] = atof(tmp);
            break;
          case 'I':
            polyfit_params_31[0] = atof(tmp);
            break;
          case 'J':
            polyfit_params_31[1] = atof(tmp);
            break;
          case 'K':
            polyfit_params_32[0] = atof(tmp);
            break;
          case 'L':
            polyfit_params_32[1] = atof(tmp);
            break;
          case 'M':
            polyfit_params_41[0] = atof(tmp);
            break;
          case 'N':
            polyfit_params_41[1] = atof(tmp);
            break;
          case 'O':
            polyfit_params_42[0] = atof(tmp);
            break;
          case 'P':
            polyfit_params_42[1] = atof(tmp);
            break;
          case 'W':
            sensor_borders[0] = atoi(tmp);
            break;
          case 'X':
            sensor_borders[1] = atoi(tmp);
            break;
          case 'Y':
            sensor_borders[2] = atoi(tmp);
            break;
          case 'Z':
            sensor_borders[3] = atoi(tmp);
            setParams(polyfit_params_11, polyfit_params_12, polyfit_params_21, polyfit_params_22, polyfit_params_31, polyfit_params_32, polyfit_params_41, polyfit_params_42);
            setBorders(sensor_borders);
            break;
        }
        inputPointer = 0;
      }
      inputMode = input;

    } else if (('0' <= input && input <= '9') || input == '.' || input == '-' || input == ',') { // 値入力
      inputValue[inputPointer] = input;
      ++inputPointer;
    }
  }

  // 光センサー入力
  int sensorDatas[4] = {analogRead(sensorPin1), analogRead(sensorPin2), analogRead(sensorPin3), analogRead(sensorPin4)};

  // 現在位置の算出
  float pos = 0; //線上が0. 機体の右へのずれが正. 両端のセンサー位置が±1
  pos = calcuPosBy4(sensorDatas);

  // モーター出力計算
  float rightPower = 0; // 右モーター出力
  float leftPower = 0; // 左モーター出力

  if (runMode == 0) {
    // 線を見失ったときは、直近のデータを元に旋回
    if (pos == 404) {
      if (direction_his == 1) {
        rightPower = basicSpeed;
        leftPower = 0;
      } else {
        rightPower = 0;
        leftPower = basicSpeed;
      }
    } else {
    // PID制御
      // direction_hisを更新
      if (pos < 0) {
        direction_his = 1;
      } else if (pos > 0) {
        direction_his = -1;
      }

      float du; // 制御量の変化
      float u; // 制御量
      du = kp * (pos - pos_1) + ki * pos + kd * (pos - 2 * pos_1 + pos_2);
      pos_2 = pos_1;
      pos_1 = pos;
      u = u + du;
      // 制御量からモーター出力量
      rightPower = basicSpeed + u;
      leftPower = basicSpeed - u;
    }
  } else if (runMode == 1) {
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
  } else if (runMode == 2) {
    rightPower = 0;
    leftPower = 0;
  }

  // モーター出力
  if (basicStatus == 1) {
    if (rightPower < 0) {
      rightPower = 0;
    } else if (rightPower > 255) {
      rightPower = 255;
    }
    if (leftPower < 0) {
      leftPower = 0;
    } else if (leftPower > 255) {
      leftPower = 255;
    }

    analogWrite(rightMoterPin, rightPower);
    analogWrite(leftMoterPin, leftPower);
  } else {
    analogWrite(rightMoterPin, 0);
    analogWrite(leftMoterPin, 0);
  }

  // 表示出力
  switch (outputMode) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      Serial.print("time:");
      Serial.print(millis());
      Serial.print(",light1:");
      Serial.print(sensorDatas[0]);
      Serial.print(",light2:");
      Serial.print(sensorDatas[1]);
      Serial.print(",light3:");
      Serial.print(sensorDatas[2]);
      Serial.print(",light4:");
      Serial.print(sensorDatas[3]);
      Serial.print(",pos:");
      Serial.println(pos);
      break;
  }
}
