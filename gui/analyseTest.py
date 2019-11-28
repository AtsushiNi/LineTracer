import numpy as np
import analyseModule

input_value = '430,450,500,520'

params11, params12, params21, params22 = analyseModule.analyseData(input_value.split(','))

# print(np.poly1d(params12)(np.arange(108,450)))
print([str(n) for n in params11])
print([str(n) for n in params12])
print([str(n) for n in params21])
print([str(n) for n in params22])
print(str(params11[0])+','+str(params11[1]))
strs = ''
for n in params11[::-1]:
    strs += str(n)
    strs += ','
print(strs)
a = '{:.7g}'.format(params11[1])
print(a)