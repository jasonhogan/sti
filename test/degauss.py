from stipy import *
from math import fabs
from math import sin
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)

supplyControl=ch(slowAnalogOut, 6)
relayTrigger = ch(slowAnalogOut, 7)

setvar('scaleFactor',1)
setvar('amplitude',4)
setvar('samplesPerSecond', 100)
setvar('frequency',1)

samplesPerCycle = samplesPerSecond/frequency
pi = 3.141592
dtDelay = 10*us

def degaussWaveform(Start):

    for i in range(0, samplesPerCycle+1,1) :
        event(supplyControl, Start+dtDelay+(i*s)/samplesPerSecond, fabs(scaleFactor*amplitude*sin(2*pi*i/samplesPerCycle)))

    event(relayTrigger, Start,0)
    event(relayTrigger, Start + (1*s)/(2*frequency),5)
    event(relayTrigger, Start + (1*s)/frequency, 0)

    event(ch(trigger,4),Start + (1*s)/frequency-100*ms,'Play')
    return Start


t0=10*us

time = t0
time = degaussWaveform(time)
