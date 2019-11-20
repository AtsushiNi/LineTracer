# excelデータからデバッグ
import openpyxl
import numpy as np
import analyseModule


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

analyseModule.analyseData(light1, light2)