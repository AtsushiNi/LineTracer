import numpy as np

def analyseData(light1, light2):
    # max,minの算出
    max = [light1.max(), light2.max()]
    min = [light1.min(), light2.min()]
    min_index = [light1.argmin(), light2.argmin()]
    data_range = [max[0]-min[0], max[1]-min[1]] # センサ値の値の幅

    print("センサ値最大")
    print(max)
    print("センサ値最小")
    print(min)

    # 座標の中心と幅を算出
    frames_par_one = abs(min_index[1] - min_index[0]) # 距離1あたりのデータのコマ数

    # 線上から線を認識しなくなるまで分析
    trans_data = np.array([min[0],0]) # 処理対象範囲を位置とセンサ値で転置した行列
    for index, data in enumerate(light1[min_index[0]::-1]):
        # 線を認識しなくなったら終わり
        if data > max[0]-data_range[0]/10:
            break
        
        np.append(trans_data, [data, index/frames_par_one],axis=0)
        
    # 近似式の係数の計算
    params = np.polyfit(trans_data[:,0], trans_data[:,1], 3)

    # もう半分
    trans_data = np.array([min[0],0]) # 処理対象範囲を位置とセンサ値で転置した行列
    for index, data in reversed(enumerate(light1[min_index[0]::-1])):
        # 線を認識しなくなったら終わり
        if data > max[0]-data_range[0]/10:
            break
        
        np.append(trans_data, [data, -index/frames_par_one], axis=0)
        
    # 近似式の係数の計算
    params = np.polyfit(trans_data[:,0], trans_data[:,1], 3)