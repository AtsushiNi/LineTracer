# include <math.h>

void calcuLikelyPos(float sensorPos, int data, float *res);

// 二つのセンサーから現在位置を割り出す
float calcuPosBy2(int rightData, int leftData, float *res) {
  float position = 0;
  float likelyPos[2][2];
  float distances[4];
  
  // それぞれのセンサーによる予想位置の算出(likelyPosを書き換える)
  calcuLikelyPos(1, rightData, likelyPos[0]);
  calcuLikelyPos(-1, leftData, likelyPos[1]);
  res[0] = likelyPos[0][0];
  res[1] = likelyPos[0][1];
  res[2] = likelyPos[1][0];
  res[3] = likelyPos[1][1];
  // 予想位置同士の差を計算
  distances[0] = fabsf(likelyPos[0][0] - likelyPos[1][0]);
  distances[1] = fabsf(likelyPos[0][0] - likelyPos[1][1]);
  distances[2] = fabsf(likelyPos[0][1] - likelyPos[1][0]);
  distances[3] = fabsf(likelyPos[0][1] - likelyPos[1][1]);
  // 差の一番小さいものを特定
  int length = sizeof(distances) / sizeof(float);
  float min = distances[0];
  int minIndex = 0;
  for(int i=1; i<length; i++) {
    if (distances[i] < min) {
      min = distances[i];
      minIndex = i;
    }
  }
  switch (minIndex) {
    case 0:
      position = (likelyPos[0][0] + likelyPos[1][0])/2.0;
      break;
    case 1:
      position = (likelyPos[0][0] + likelyPos[1][1])/2.0;
      break;
    case 2:
      position = (likelyPos[0][1] + likelyPos[1][0])/2.0;
      break;
    case 3:
      position = (likelyPos[0][1] + likelyPos[1][1])/2.0;
      break;
  }

  return position;
}

// センサーの位置とそのセンサーのデータから機体の予想位置を二つ算出する sensorPos: -1~+1. 右が正
void calcuLikelyPos(float sensorPos, int data, float likelyPos[2]) {
  likelyPos[0] = 1 - data/1000.0;
  likelyPos[1] = -likelyPos[0];

  likelyPos[0] += sensorPos;
  likelyPos[1] += sensorPos;

  if (data < 50) {
    likelyPos[0] = 100;
    likelyPos[1] = -100;
  }
}

// 4つのセンサーから現在位置を割り出す。主なセンサー1つと周りのセンサーの比から算出する簡単バージョン
// light1が一番右のセンサー
// sensor_ranges: センサーの閾値
float calcuPosBy4Simple(int light1, int light2, int light3, int light4, int *sensor_ranges) {
  // 線を認識していない場合は404を返す
  if (light1 > (sensor_ranges[0][1]-(sensor_ranges[0][1]-sensor_ranges[0][0])/10.0)
      && light2 > (sensor_ranges[1][1]-(sensor_ranges[1][1]-sensor_ranges[1][0])/10.0)
      && light3 > (sensor_ranges[2][1]-(sensor_ranges[2][1]-sensor_ranges[2][0])/10.0)
      && light4 > (sensor_ranges[3][1]-(sensor_ranges[3][1]-sensor_ranges[3][0])/10.0)) {
        return 404
      }
  // 4つのセンサーの中で一番値が小さいものを求める
  if (light1 < light2 && light1 < light3 && light1 < light4) {
    // light1が一番小さい
    return -1
  } else if (light2 < light3 && light2 < light4) {
    // light2が一番小さい
    if (light1 < light3) {
      return -0.5
    } else {
      return -0.167
    }
  } else if (light3 < light4) {
    // light3が一番小さい
    if (light2 < light4) {
      return 0.167
    } else {
      return 0.5
    }
  } else {
    // light4が一番小さい
    return 1
  }
}
