# コマンドの送信用gui
from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.clock import Clock
from kivy.garden.matplotlib.backend_kivyagg import FigureCanvasKivyAgg
from kivy.core.window import Window

import openpyxl

import numpy as np
import matplotlib.pyplot as plt
import matplotlib
# Kivy 上で Matplotlib を使うために必要な準備
matplotlib.use('module://kivy.garden.matplotlib.backend_kivy')
import re
import serial
import threading

import analyseModule

# 定数
FRAME_TIME = 10 # 更新間隔(ms)
FRAME_NUM = 200 # 画面に表示するデータ数
ANALYSE_LOG_FILE_NAME = 'logs/sampleLogBy4Sensors.xlsx' # 制御パラメータ算出に使用するログファイル
SAVE_LOG_FILE_NAME = 'logs/log.xlsx' # ログを記録するファイル名

commands = [] # 送信待ちコマンド
input_value = '' # 入力中のコマンド

# グラフ描画のstart/stop
is_graph_updating = True

# グラフのデータ
light1 = np.zeros(FRAME_NUM)
light2 = np.zeros(FRAME_NUM)
light3 = np.zeros(FRAME_NUM)
light4 = np.zeros(FRAME_NUM)
pos = np.zeros(FRAME_NUM)
time = np.zeros(FRAME_NUM)

class MainScreen3(BoxLayout):
    def handle_change(self, value):
        global input_value
        input_value = value.decode()

    def handle_submit(self, value):
        global commands
        global input_value
        if ',' in input_value:
            # センサーデータの解析
            params11, params12, params21, params22, params31, params32, params41, params42 = analyseModule.analyseData(input_value.split(','), ANALYSE_LOG_FILE_NAME)
            # 右のセンサの前半から順に近似式の係数を送信していく。低い次数から順。指数表記になるような小さすぎる高次の係数は無視
            # commands.append("m")
            # for n in params11[::-1]:
            #     if not ('e' in str(n)):
            #         commands.append(str(n))
            #         commands.append(',')
            # for n in params12[::-1]:
            #     if not ('e' in str(n)):
            #         commands.append(str(n))
            #         commands.append(',')
            # for n in params21[::-1]:
            #     if not ('e' in str(n)):
            #         commands.append(str(n))
            #         commands.append(',')
            # for n in params22[::-1]:
            #     if not ('e' in str(n)):
            #         commands.append(str(n))
            #         commands.append(',')
            # commands.append('a')
            # print(commands)

            # 係数の送信
            commands.append("A")
            commands.append(str(params11[-1]))
            commands.append("a")
            commands.append("B")
            commands.append(str(params11[-2]))
            commands.append("a")
            commands.append("C")
            commands.append(str(params12[-1]))
            commands.append("a")
            commands.append("D")
            commands.append(str(params12[-2]))
            commands.append("a")
            commands.append("E")
            commands.append(str(params21[-1]))
            commands.append("a")
            commands.append("F")
            commands.append(str(params21[-2]))
            commands.append("a")
            commands.append("G")
            commands.append(str(params22[-1]))
            commands.append("a")
            commands.append("H")
            commands.append(str(params22[-2]))
            commands.append("a")
            commands.append("I")
            commands.append(str(params31[-1]))
            commands.append("a")
            commands.append("J")
            commands.append(str(params31[-2]))
            commands.append("a")
            commands.append("K")
            commands.append(str(params32[-1]))
            commands.append("a")
            commands.append("L")
            commands.append(str(params32[-2]))
            commands.append("a")
            commands.append("M")
            commands.append(str(params41[-1]))
            commands.append("a")
            commands.append("N")
            commands.append(str(params41[-2]))
            commands.append("a")
            commands.append("O")
            commands.append(str(params42[-1]))
            commands.append("a")
            commands.append("P")
            commands.append(str(params42[-2]))
            commands.append("a")

            # センサの閾値の送信
            sensor_borders = input_value.split(',')
            commands.append("W")
            commands.append(str(int(sensor_borders[0])))
            commands.append("a")
            commands.append("X")
            commands.append(str(int(sensor_borders[1])))
            commands.append("a")
            commands.append("Y")
            commands.append(str(int(sensor_borders[2])))
            commands.append("a")
            commands.append("Z")
            commands.append(str(int(sensor_borders[3])))
            commands.append("a")
        else:
            commands.append(input_value)
            print(input_value)

    def handle_start(self):
        global commands
        global is_graph_updating
        commands.append("b1a")
        is_graph_updating = True
        print("start")

    def handle_stop(self):
        global commands
        global is_graph_updating
        global light1
        global light2
        global light3
        global light4
        global time
        global FRAME_NUM

        commands.append("b0a")
        is_graph_updating = False

        # 最初の200行は0で埋まっているので削除
        while time[0] == 0:
            light1 = np.delete(light1, 0)
            light2 = np.delete(light2, 0)
            light3 = np.delete(light3, 0)
            light4 = np.delete(light4, 0)
            time = np.delete(time, 0)

        # Excelにログ保存
        wb = openpyxl.load_workbook(SAVE_LOG_FILE_NAME)
        sheet = wb['Sheet1']
        for t in range(np.size(time)):
            sheet.cell(row=t+1,column=1,value=t+1)
            sheet.cell(row=t+1,column=2,value=time[t])
            sheet.cell(row=t+1,column=3,value=light1[t])
            sheet.cell(row=t+1,column=4,value=light2[t])
            sheet.cell(row=t+1,column=5,value=light3[t])
            sheet.cell(row=t+1,column=6,value=light4[t])
        wb.save(SAVE_LOG_FILE_NAME)

        # データ初期化
        light1 =light2 = light3 = light4 = pos = time = np.zeros(FRAME_NUM)
        print("stop")

    def handle_mode_test(self):
        global commands
        commands.append("c2a")
        print("testMode")

    def handle_mode_run(self):
        global commands
        commands.append("c0a")
        print("runMode")

    def handle_kp_change(self, value):
        global commands
        commands.append("j"+str(value)+"a")
        print("KP: "+str(value))

    def handle_ki_change(self, value):
        global commands
        commands.append("k"+str(value)+"a")
        print("KI: "+str(value))

    def handle_kd_change(self, value):
        global commands
        commands.append("l"+str(value)+"a")
        print("KD: "+str(value))

class GraphView(BoxLayout):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        global FRAME_TIME
        global FRAME_NUM

        # 初期化に用いるデータ
        x = np.linspace(1,FRAME_NUM,FRAME_NUM)
        y = np.zeros(FRAME_NUM)

        # Figure, Axis を保存しておく
        self.fig, self.ax = plt.subplots(2, facecolor="0.1")
        self.ax[0].tick_params(axis='x', colors="0.8")
        self.ax[0].tick_params(axis='y', colors="0.8")
        self.ax[0].set_facecolor((0.4, 0.4, 0.4, 1))
        self.ax[0].grid(axis='y')
        self.ax[0].set_ylim([0, 500])
        self.ax[1].tick_params(axis='x', colors="0.8")
        self.ax[1].tick_params(axis='y', colors="0.8")
        self.ax[1].set_facecolor((0.4, 0.4, 0.4, 1))

        # 最初に描画したときの Line も保存しておく
        self.line11, = self.ax[0].plot(x, y, label="light1")
        self.line12, = self.ax[0].plot(x, y, label="light2")
        self.line13, = self.ax[0].plot(x, y, label="light3")
        self.line14, = self.ax[0].plot(x, y, label="light4")

        self.line21, = self.ax[1].plot(x, y)

        # ウィジェットとしてグラフを追加する
        widget = FigureCanvasKivyAgg(self.fig)
        self.add_widget(widget)

        # frame_time秒ごとに表示を更新するタイマーを仕掛ける
        Clock.schedule_interval(self.update_view, FRAME_TIME/1000)

    def update_view(self, *args, **kwargs):

        global light1
        global light2
        global light3
        global light4
        global pos
        global is_graph_updating

        # is_graph_updating == Falseならグラフ更新しない
        if not is_graph_updating:
            return

        # データを更新する
        x = np.linspace(1,FRAME_NUM,FRAME_NUM)
        y11 = light1[-FRAME_NUM:]
        y12 = light2[-FRAME_NUM:]
        y13 = light3[-FRAME_NUM:]
        y14 = light4[-FRAME_NUM:]

        y21 = pos[-FRAME_NUM:]

        # Line にデータを設定する
        self.line11.set_data(x, y11)
        self.line12.set_data(x, y12)
        self.line13.set_data(x, y13)
        self.line14.set_data(x, y14)
        self.line21.set_data(x, y21)
        # グラフの見栄えを調整する
        self.ax[0].relim()
        self.ax[0].autoscale_view()
        self.ax[0].legend(loc='upper left')
        self.ax[1].relim()
        self.ax[1].autoscale_view()
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
        Window.size = (1000,700)
        Window.top = 50
        return screen

# シリアル通信受信クラス
class SerialClient():
    def start(self):
        serial_thread = threading.Thread(target=self.serial_method, daemon=True)
        serial_thread.start()
    
    # シリアル通信用スレッドの実装部
    def serial_method(self):
        global commands
        print_flag = 0
        ser = serial.Serial("COM5",9600) # シリアル通信

        while True:
            line = ser.readline()
            line = line.decode().rstrip('\r\n')
            print_flag += 1
            if print_flag > 50:
                print(line)
                print_flag = 0
            receives = re.split(',', line)
            for receive in receives:
                x = re.split(':', receive)
                if x[0] == 'light1':
                    global light1
                    light1 = np.append(light1, int(x[1]))
                elif x[0] == 'light2':
                    global light2
                    light2 = np.append(light2, int(x[1]))
                elif x[0] == 'light3':
                    global light3
                    light3 = np.append(light3, int(x[1]))
                elif x[0] == 'light4':
                    global light4
                    light4 = np.append(light4, int(x[1]))
                elif x[0] == 'pos':
                    global pos
                    pos = np.append(pos, float(x[1]))
                elif x[0] == 'time':
                    global time
                    time = np.append(time, int(x[1]))

            # 送信
            if len(commands) > 0:
                for command in commands:
                    ser.write(command.encode())
                commands = []   

if __name__ == '__main__':
    Controller3App().run()
