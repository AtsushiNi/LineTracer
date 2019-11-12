# include <math.h>

void calcuLikelyPos(float sensorPos, int data);

// 二つのセンサーから現在位置を割り出す
float calcuPosBy2(int rightData, int leftData) {
  float position = 0;
  float likelyPos[2][2];
  float distances[4];
  
  // それぞれのセンサーによる予想位置の算出(likelyPosを書き換える)
  calcuLikelyPos(1, rightData, likelyPos[0]);
  calcuLikelyPos(-1, leftData, likelyPos[1]);
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
