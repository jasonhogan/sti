ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

variables.append('dtRabiSequence')

tLow = 10*us
tHigh = 2010*us
dt = 20*us
trialNumber = (tHigh - tLow) / dt + 1

for i in range(0, trialNumber) :
    experiments.append([tLow + dt*i])