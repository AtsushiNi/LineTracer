import numpy as np
import analyseModule

input_value = '190,250,240,190'

params11, params12, params21, params22, params31, params32, params41, params42 = analyseModule.analyseData(input_value.split(','))

# print(np.poly1d(params12)(np.arange(108,450)))
print([str(n) for n in params11])
print([str(n) for n in params12])
print([str(n) for n in params21])
print([str(n) for n in params22])
print([str(n) for n in params31])
print([str(n) for n in params32])
print([str(n) for n in params41])
print([str(n) for n in params42])