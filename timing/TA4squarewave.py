from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

include('channels.py')
include('experimentalParameters.py')

squareWaveOn = 50*ms
squareWaveOff = 50*ms

voltageTA = ta4MotVoltage

#Initialization Setting
tStart = 100*us
time = tStart

for i in range(0,10) :    
    event(digitalSynch, time, 0)
    event(TA4, time,0.06)    # TA off
    time += squareWaveOn
    event(digitalSynch, time, 1)
    event(TA4, time, voltageTA)     # TA on
    time += squareWaveOff
