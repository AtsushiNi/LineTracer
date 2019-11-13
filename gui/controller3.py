# コマンドの送信用gui
from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.clock import Clock

import re
import serial
import threading

commands = [] # 送信待ちコマンド
data = []

class MainScreen3(BoxLayout):
    def on_enter(self, value):
        global commands
        commands.append(value)
        print(value)

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