ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

variables.append('deltaFreq')

tLow = 5000
tHigh = 104000
dt = 1000
trialNumber = (tHigh - tLow) / dt + 1

for i in range(0, trialNumber) :
    experiments.append([tLow + dt*i])