from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0
urad = 0.000001

include('channels.py')
include('experimentalParameters.py')

squareWaveOn = 500*ms
squareWaveOff =500*ms

voltageTA = taZMotVoltage/2
#voltageTA = TA4LatticeCurrent #taZMotVoltage

#taChannel = TAZ
taChannel = TAZ

#Initialization Setting
tStart = 100*us
time = tStart

for i in range(0,11) :    
    event(taChannel, time,0.06)    # TA off
    time += squareWaveOn
#    event(digitalSynch, time, 1)
    event(digitalSynch, time-10*us, 0)
    event(digitalSynch, time, 1)
    event(digitalSynch, time+10*us, 0)
    event(taChannel, time, voltageTA)     # TA on
    time += squareWaveOff

event(taChannel, time,0.06)    # TA off

