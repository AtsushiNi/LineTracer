# コマンドの送信用gui
from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.clock import Clock
from kivy.garden.matplotlib.backend_kivyagg import FigureCanvasKivyAgg

import numpy as np
import matplotlib.pyplot as plt
import matplotlib
# Kivy 上で Matplotlib を使うために必要な準備
matplotlib.use('module://kivy.garden.matplotlib.backend_kivy')
import re
import serial
import threading

# 定数
FRAME_TIME = 10 # 更新間隔(ms)
FRAME_NUM = 100 # 画面に表示するデータ数

commands = [] # 送信待ちコマンド
data = np.array(FRAME_NUM)

class MainScreen3(BoxLayout):
    def on_enter(self, value):
        global commands
        commands.append(value)
        print(value)

class GraphView(BoxLayout):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        global FRAME_TIME
        global FRAME_NUM

        # 初期化に用いるデータ
        x = np.linspace(1,FRAME_NUM,FRAME_NUM)
        y = np.zeros(FRAME_NUM)

        # Figure, Axis を保存しておく
        self.fig, self.ax = plt.subplots()
        # 最初に描画したときの Line も保存しておく
        self.line, = self.ax.plot(x, y)

        # ウィジェットとしてグラフを追加する
        widget = FigureCanvasKivyAgg(self.fig)
        self.add_widget(widget)

        # frame_time秒ごとに表示を更新するタイマーを仕掛ける
        Clock.schedule_interval(self.update_view, FRAME_TIME)

    def update_view(self, *args, **kwargs):

        global data

        # データを更新する
        x = np.linspace(1,FRAME_NUM,FRAME_NUM)
        y = data[-FRAME_NUM]

        # Line にデータを設定する
        self.line.set_data(x, y)
        # グラフの見栄えを調整する
        self.ax.relim()
        self.ax.autoscale_view()
        # 再描画する
        self.fig.canvas.draw()
        self.fig.canvas.flush_events()

class Controller3App(App):
    def __init__(self, **kwargs):
        super(Controller3App, self).__init__(**kwargs)

    def build(self):
        serialClient = SerialClient()
        serialClient.start()
        screen = MainScreen3()
        return screen

# シリアル通信受信クラス
class SerialClient():
    def start(self):
        serial_thread = threading.Thread(target=self.serial_method, daemon=True)
        serial_thread.start()
    
    # シリアル通信用スレッドの実装部
    def serial_method(self):
        global data
        global commands
        ser = serial.Serial("COM5",9600) # シリアル通信

        while True:
            line = ser.readline()
            line = line.decode()
            numbers = re.findall('[0-9]+', line)
            if len(numbers) > 0: # 通信に数字が含まれていなければなにもしない
                data.append(int(numbers[0]))
                # if len(numbers) > 1: # 複数のグラフ描画
                    # data2.append(int(numbers[1]))
                #     print('(' + numbers[0] + ',' + numbers[1] + ')')
                # else:
                #     a=''
                    # print(numbers[0])
            if len(commands) > 0:
                for command in commands:
                    ser.write(command)
                commands = []   

if __name__ == '__main__':
    Controller3App().run()