# LineTracer

proto/ : トレーサー本体のプログラム
gui/ : pc側のguiプログラム

# Usage

実行前にgui/logsにlog.xlsxを作ってExcelを閉じておく

実行方法
```bash
cd gui
cd scripts
activate.bat
cd ..
python controller3.py
```
python仮想環境の停止
```
deactivate
```

センサーの初期化
- gui/controller3を起動
- startボタンで開始、stopボタンで終了
- 線の左から右に、一定の速度で車体を移動させる
- センサーの閾値を,区切りで入力→解析開始

# Command

Arduinoに送るコマンド一覧
コマンド+数字+'a'で実行される
- a : コマンド入力の実行・リセット
- b : 基本ステータス　0:待機, 1: 走行
- c : 走行モード　0:ライントレース, 1:ラジコン, 2:テスト
- d : ラジコンモードの進路方向　0:直進, 1:右, 2: 左
- e : 走行スピード
- f : ラジコンモードでの左右減速比
- g : モーター出力値を出力するかどうか 0:しない, 1:する
- h : 光センサーの値を出力するかどうか 0:しない, 1:する
- i : 表示出力の様式　0:ArduinoIDE, 1:Android, 2:Python
- j : 比例制御のパラメータ
- k : 積分制御のパラメータ
- l : 微分制御のパラメータ
- A~ : センサ値の解析用関数の係数
- W~ : センサの閾値
