# シリアル通信を取得して数値を取り出し、グラフ化
import re
import serial
from matplotlib import pyplot as plt
from matplotlib import animation
import numpy as np
import math
import threading

from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.button import Button

data = []
data2 = []

class MainScreen(BoxLayout):
    pass

class ControllerApp(App):
    def __init__(self, **kwargs):
        super(ControllerApp, self).__init__(**kwargs)

    def build(self):
        return MainScreen()    

# シリアル通信用スレッドの実装部
def serial_method():
    ser = serial.Serial("COM6",9600) # シリアル通信

    while True:
        line = ser.readline()
        line = line.decode()
        numbers = re.findall('[0-9]+', line)
        if len(numbers) > 0: # 通信に数字が含まれていなければなにもしない
            data.append(int(numbers[0]))
            if len(numbers) > 1: # 複数のグラフ描画
                data2.append(int(numbers[1]))
                print('(' + numbers[0] + ',' + numbers[1] + ')')
            else:
                print(numbers[0])

# グラフ描画用スレッドの実装部
# グラフ設定をし、描画を開始させる
def graph_method():
    fig = plt.figure(figsize=(10,6)) # 描画領域
    x = []
    y = []
    y2 = []

    params = {
        'fig': fig,
        'func': _update,  # グラフを更新する関数
        'fargs': (x, y, y2),  # 関数の引数 (フレーム番号を除く)
        'interval': 2,  # 更新間隔 (ミリ秒)
        'frames': np.arange(0, 10, 0.05),  # フレーム番号を生成するイテレータ
        'repeat': True,  # 繰り返さす
    }
    # anime = animation.FuncAnimation(**params)
    # plt.show() # グラフを表示する

    ControllerApp().run()

# graph_method()から更新間隔で呼び出される関数。
# data[]から値を読み込んでグラフにセットする
def _update(frame, x, y, y2):
    plt.cla() # 現在のグラフを消去
    x.append(frame)

    if len(data) > 0:
        y.append(data[-1]) # 実行直後でまだデータが送られてきていないときは値は0とする
    else:
        y.append(0)

    if len(data2) > 0:
        y2.append(data2[-1])
    else:
        y2.append(0)

    if len(data) > 100: # グラフを横にスライドさせていく
        x.pop(0)
        y.pop(0)
        y2.pop(0)

    plt.plot(x, y)
    plt.plot(x, y2)

def main():
    graph_thread = threading.Thread(target=graph_method)
    serial_thread = threading.Thread(target=serial_method)
    serial_thread.setDaemon(True) #メインスレッドが終了するとシリアル通信用スレッドを切る

    graph_thread.start()
    serial_thread.start()

if __name__ == '__main__':
    main()

