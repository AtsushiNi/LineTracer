import numpy as np
import openpyxl

# 線形近似の次数
polyfit_index_num = 1

def analyseData(borders):
    # センサー値配列の取得
    light1, light2 = getDataFromExcel()

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

    # 線上から線を認識しなくなるまで分析 light1の前半
    trans_data = np.array([[min[0],0]]) # 処理対象範囲を位置とセンサ値で転置した行列
    for index, data in enumerate(light1[min_index[0]::-1]):
        # 線を認識しなくなったら終わり
        if data > int(borders[0]):
            break
        
        trans_data = np.append(trans_data, [[data, -index/frames_par_one]],axis=0)
        
    # 近似式の係数の計算
    params11 = np.polyfit(trans_data[:,0], trans_data[:,1], polyfit_index_num)

    # 線上から線を認識しなくなるまで分析 light1の後半
    trans_data = np.array([[min[0],0]]) # 処理対象範囲を位置とセンサ値で転置した行列
    for index, data in enumerate(light1[min_index[0]:-1:]):
        # 線を認識しなくなったら終わり
        if data > int(borders[1]):
            break
        
        trans_data = np.append(trans_data, [[data, index/frames_par_one]], axis=0)
        
    # 近似式の係数の計算
    params12 = np.polyfit(trans_data[:,0], trans_data[:,1], polyfit_index_num)

    # 線上から線を認識しなくなるまで分析 light2の前半
    trans_data = np.array([[min[0],0]]) # 処理対象範囲を位置とセンサ値で転置した行列
    for index, data in enumerate(light2[min_index[1]::-1]):
        # 線を認識しなくなったら終わり
        if data > int(borders[2]):
            break
        
        trans_data = np.append(trans_data, [[data, -index/frames_par_one]],axis=0)
        
    # 近似式の係数の計算
    params21 = np.polyfit(trans_data[:,0], trans_data[:,1], polyfit_index_num)

    # 線上から線を認識しなくなるまで分析 light2の後半
    trans_data = np.array([[min[0],0]]) # 処理対象範囲を位置とセンサ値で転置した行列
    for index, data in enumerate(light2[min_index[1]:-1:]):
        # 線を認識しなくなったら終わり
        if data > int(borders[3]):
            break
        
        trans_data = np.append(trans_data, [[data, index/frames_par_one]], axis=0)
        
    # 近似式の係数の計算
    params22 = np.polyfit(trans_data[:,0], trans_data[:,1], polyfit_index_num)

    return params11, params12, params21, params22

def getDataFromExcel():
    wb = openpyxl.load_workbook('logs/sampleLog.xlsx')
    sheet = wb['Sheet1']

    # 先頭のデータ取得
    index = np.array([sheet['A201'].value])
    time = np.array([sheet['B201'].value])
    light1 = np.array([sheet['C201'].value])
    light2 = np.array([sheet['D201'].value])
    # 範囲データ取得
    sheet_range = sheet['A202':'D305']
    # 範囲データを順次処理
    for row in sheet_range:
        index = np.append(index, [row[0].value])
        time = np.append(time, [row[1].value])
        light1 = np.append(light1, [row[2].value])
        light2 = np.append(light2, [row[3].value])

    # ロードしたExcelファイルを閉じる
    wb.close()

    return light1, light2