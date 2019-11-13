# コマンドの送信用gui
from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.clock import Clock

import serial
import threading

commands = [] # 送信待ちコマンド

class MainScreen(BoxLayout):
    command_line = 'aaaa'

    def start(self):
        Clock.schedule_interval(self.updateData, 0.1)

    def updateData(self, dt):
        if data == []:
            return

        command_line = str(data[-1])

    def add_func(self, a, b):
        return (str(a) + "\n" + str(b))
        
class Controller3App(App):
    def __init__(self, **kwargs):
        super(Controller2App, self).__init__(**kwargs)

    def build(self):
        serialClient = SerialClient()
        serialClient.start()
        screen = MainScreen()
        screen.start()
        return screen

# シリアル通信受信クラス
class SerialClient():
    def start(self):
        serial_thread = threading.Thread(target=self.serial_method, daemon=True)
        serial_thread.start()
    
    # シリアル通信用スレッドの実装部
    def serial_method(self):
        ser = serial.Serial("COM5",9600) # シリアル通信

        while True:
            line = ser.readline()
            line = line.decode()
            numbers = re.findall('[0-9]+', line)
            if len(numbers) > 0: # 通信に数字が含まれていなければなにもしない
                data.append(int(numbers[0]))
                if len(numbers) > 1: # 複数のグラフ描画
                    data2.append(int(numbers[1]))
                #     print('(' + numbers[0] + ',' + numbers[1] + ')')
                # else:
                #     a=''
                    # print(numbers[0])
            if len(commands) > 0:
                for command in commands:
                    ser.write(command)

if __name__ == '__main__':
    Controller3App().run()