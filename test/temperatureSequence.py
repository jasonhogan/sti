ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

variables.append('holdTimeSequence')

tLow = 1*s
tHigh = 10*s
dt = 2*s
trialNumber = (tHigh - tLow) / dt + 1

for i in range(0, trialNumber) :
    experiments.append([tLow + dt*i])
