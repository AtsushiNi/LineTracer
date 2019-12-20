# include <math.h>

float calcu_polyfit_params[4][2][2] = {
  {
    {0.255756532, -0.003234247},
    {-0.26329114, 0.0033770307}
  },
  {
    {0.189936256, -0.0023355},
    {-0.20201530, 0.00210843}
  },
  {
    {0.195091001, -0.00226165},
    {-0.28384191, 0.003158531}
  },
  {
    {0.12523096, -0.002790527},
    {-0.1336090, 0.0025153852}
  }
};
int calcu_sensor_borders[4] = {
  170,
  230,
  220,
  165
};

void calcuLikelyPos(float sensorPos, int data, float *res);
void calcuLikelyPosFor4(float sensorPos, int data, float *res);

// 近似式の係数をセットする
void setParams(float *params_11, float *params_12, float *params_21, float *params_22, float *params_31, float *params_32, float *params_41, float *params_42) {
  for(int i = 0; i < 2; i++ ){
    calcu_polyfit_params[0][0][i] = params_11[i];
    calcu_polyfit_params[0][1][i] = params_12[i];
    calcu_polyfit_params[1][0][i] = params_21[i];
    calcu_polyfit_params[1][1][i] = params_22[i];
    calcu_polyfit_params[2][0][i] = params_31[i];
    calcu_polyfit_params[2][1][i] = params_32[i];
    calcu_polyfit_params[3][0][i] = params_41[i];
    calcu_polyfit_params[3][1][i] = params_42[i];
  }

}

// 各センサーが反応しているかどうかの境界を設定
void setBorders(int border1, int border2, int border3, int border4) {
  calcu_sensor_borders[0] = border1;
  calcu_sensor_borders[1] = border2;
  calcu_sensor_borders[2] = border3;
  calcu_sensor_borders[3] = border4;
}

// 4つのセンサーから現在位置を割り出す。
// sensorDatas: 各センサーの値。右のセンサーから順。
// polyfit_params: python側で計算した、センサ値-現在位置の近似式の係数。[センサー][{0次,1次}]
// sensor_ranges: センサーの閾値
float calcuPosBy4(int *sensorDatas) {
  // 各センサーが線を認識しているかどうか調べる
  int active_sensor_num = 0;
  int active_sensors[4] = {0, 0, 0, 0};
  for (int i = 0; i < 4; i++){
    if (sensorDatas[i] < calcu_sensor_borders[i]) {
      active_sensors[active_sensor_num] = i;
      active_sensor_num++;
    }
  }
  Serial.print("case:");
  Serial.print(active_sensor_num);
  Serial.print(",light1:");
  Serial.print(sensorDatas[0]);
  Serial.print(",light2:");
  Serial.print(sensorDatas[1]);
  Serial.print(",light3:");
  Serial.print(sensorDatas[2]);
  Serial.print(",light4:");
  Serial.print(sensorDatas[3]);
  Serial.print(",");

  if (active_sensor_num == 0) {
    // 1つも認識しているセンサーがなければ404を返す
    return 404;
  } else if (active_sensor_num == 1) {
    // TODO: 外側のセンサのみ反応したときの返り値を適当な値にする
    // 1つしか反応していない→中央のセンサならセンサ位置、外側のセンサなら±1.2を返す
    if (sensorDatas[0] < calcu_sensor_borders[0]) {
      return -1.2;
    } else if (sensorDatas[1] < calcu_sensor_borders[1]) {
      return -1/3.0;
    } else if (sensorDatas[2] < calcu_sensor_borders[2]) {
      return 1/3.0;
    } else if (sensorDatas[3] < calcu_sensor_borders[3]) {
      return 1.2;
    }
  } else if (active_sensor_num == 3) {
    // TODO: センサー三つが同時に反応しないようなら、ここは404を返すようにする
    // TODO: とりあえず値の小さい二つのセンサーで処理するようにするが、三つとも使った方がいいのかも
    if (sensorDatas[active_sensors[0]] > sensorDatas[active_sensors[1]] && sensorDatas[active_sensors[0]] > sensorDatas[active_sensors[2]]) {
      active_sensors[0] = active_sensors[2];
    }
    if (sensorDatas[active_sensors[1]] > active_sensors[2]) {
      active_sensors[1] = active_sensors[2];
    }
  } else if (active_sensor_num == 4) {
    return 404;
  }

  if (active_sensor_num == 2 || active_sensor_num == 3) {
    // 2つ以上が反応していれば候補を２つずつ出して比較
    float sensorPos[4] = {-1.0, -1/3.0, 1/3.0, 1.0}; // 各センサー位置
    float likelyPoses[2][2]; // 位置候補
    for (int i = 0; i < 2; i++){
      likelyPoses[i][0] = calcuLikelyPosFor4(sensorPos[active_sensors[i]], sensorDatas[active_sensors[i]], calcu_polyfit_params[active_sensors[i]][0][0], calcu_polyfit_params[active_sensors[i]][0][1]);
      likelyPoses[i][1] = calcuLikelyPosFor4(sensorPos[active_sensors[i]], sensorDatas[active_sensors[i]], calcu_polyfit_params[active_sensors[i]][1][0], calcu_polyfit_params[active_sensors[i]][1][1]);
    }
    // 位置候補同士の差を計算
    float distances[4];
    float position;
    distances[0] = fabsf(likelyPoses[0][0] - likelyPoses[1][0]);
    distances[1] = fabsf(likelyPoses[0][0] - likelyPoses[1][1]);
    distances[2] = fabsf(likelyPoses[0][1] - likelyPoses[1][0]);
    distances[3] = fabsf(likelyPoses[0][1] - likelyPoses[1][1]);
    // 差の小さいもの同士の間を出力
    float min = distances[0];
    int minIndex = 0;
    for(int i=1; i<4; i++) {
      if (distances[i] < min) {
        min = distances[i];
        minIndex = i;
      }
    }
    switch (minIndex) {
      case 0:
        position = (likelyPoses[0][0] + likelyPoses[1][0])/2.0;
        break;
      case 1:
        position = (likelyPoses[0][0] + likelyPoses[1][1])/2.0;
        break;
      case 2:
        position = (likelyPoses[0][1] + likelyPoses[1][0])/2.0;
        break;
      case 3:
        position = (likelyPoses[0][1] + likelyPoses[1][1])/2.0;
        break;
      default:
        position = 0;
    }
    return position;
  }
}

// センサーの位置とそのセンサーのデータから機体の予想位置を算出する
float calcuLikelyPosFor4(float sensorPos, int data, float params_0, float params_1) {
  return params_1 * data + params_0 + sensorPos;
}

// 4つのセンサーから現在位置を割り出す。主なセンサー1つと周りのセンサーの比から算出する簡単バージョン
// light1が一番右のセンサー
// sensor_ranges: センサーの閾値
// float calcuPosBy4Simple(int light1, int light2, int light3, int light4, int *sensor_ranges) {
//   // 線を認識していない場合は404を返す
//   if (light1 > (sensor_ranges[0][1]-(sensor_ranges[0][1]-sensor_ranges[0][0])/10.0)
//       && light2 > (sensor_ranges[1][1]-(sensor_ranges[1][1]-sensor_ranges[1][0])/10.0)
//       && light3 > (sensor_ranges[2][1]-(sensor_ranges[2][1]-sensor_ranges[2][0])/10.0)
//       && light4 > (sensor_ranges[3][1]-(sensor_ranges[3][1]-sensor_ranges[3][0])/10.0)) {
//         return 404
//   }
//   // 4つのセンサーの中で一番値が小さいものを求める
//   if (light1 < light2 && light1 < light3 && light1 < light4) {
//     // light1が一番小さい
//     return -1
//   } else if (light2 < light3 && light2 < light4) {
//     // light2が一番小さい
//     if (light1 < light3) {
//       return -0.5
//     } else {
//       return -0.167
//     }
//   } else if (light3 < light4) {
//     // light3が一番小さい
//     if (light2 < light4) {
//       return 0.167
//     } else {
//       return 0.5
//     }
//   } else {
//     // light4が一番小さい
//     return 1
//   }
// }

// 二つのセンサーから現在位置を割り出す
// float calcuPosBy2(int rightData, int leftData, float *res) {
//   float position = 0;
//   float likelyPos[2][2];
//   float distances[4];
  
//   // それぞれのセンサーによる予想位置の算出(likelyPosを書き換える)
//   calcuLikelyPos(1, rightData, likelyPos[0]);
//   calcuLikelyPos(-1, leftData, likelyPos[1]);
//   res[0] = likelyPos[0][0];
//   res[1] = likelyPos[0][1];
//   res[2] = likelyPos[1][0];
//   res[3] = likelyPos[1][1];
//   // 予想位置同士の差を計算
//   distances[0] = fabsf(likelyPos[0][0] - likelyPos[1][0]);
//   distances[1] = fabsf(likelyPos[0][0] - likelyPos[1][1]);
//   distances[2] = fabsf(likelyPos[0][1] - likelyPos[1][0]);
//   distances[3] = fabsf(likelyPos[0][1] - likelyPos[1][1]);
//   // 差の一番小さいものを特定
//   int length = sizeof(distances) / sizeof(float);
//   float min = distances[0];
//   int minIndex = 0;
//   for(int i=1; i<length; i++) {
//     if (distances[i] < min) {
//       min = distances[i];
//       minIndex = i;
//     }
//   }
//   switch (minIndex) {
//     case 0:
//       position = (likelyPos[0][0] + likelyPos[1][0])/2.0;
//       break;
//     case 1:
//       position = (likelyPos[0][0] + likelyPos[1][1])/2.0;
//       break;
//     case 2:
//       position = (likelyPos[0][1] + likelyPos[1][0])/2.0;
//       break;
//     case 3:
//       position = (likelyPos[0][1] + likelyPos[1][1])/2.0;
//       break;
//     default:
//       position = 0;
//   }

//   return position;
// }

// センサーの位置とそのセンサーのデータから機体の予想位置を二つ算出する sensorPos: -1~+1. 右が正
// void calcuLikelyPos(float sensorPos, int data, float likelyPos[2]) {
//   likelyPos[0] = 1 - data/1000.0;
//   likelyPos[1] = -likelyPos[0];

//   likelyPos[0] += sensorPos;
//   likelyPos[1] += sensorPos;

//   if (data < 50) {
//     likelyPos[0] = 100;
//     likelyPos[1] = -100;
//   }
// }
