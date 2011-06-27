from stipy import *
from math import fabs

 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

include('channels.py')
include('motFunction.py')
include('andorCameraFunctions.py')
include('evaporativeCoolingFunction.py')
include('repumpFunction.py')
include('depumpFunction.py')

#setvar('imageCropVector',(514, 393 ,250))
setvar('imageCropVector',(500, 500, 490))

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 1*ms)

#setvar('holdTimeSequence', 1*s)
#setvar('holdTime', holdTimeSequence)
setvar('holdTime', 0.1*s)

setvar('vca1', 6)
setvar('f0', 150)
setvar('f1', 100)
setvar('dtRamp1', 5*s)
setvar('vca2', 5.5)
setvar('f2', 50)
setvar('dtRamp2', 5*s)
setvar('vca3', 5.00)
setvar('f3', 15)
setvar('dtRamp3', 3*s)


setvar('MOTLoadTime', 5*s )
setvar('magneticTrap', True)
setvar('opticallyPump', True)
setvar('usePreCharge', False)
setvar('evaporateAtoms', False)

setvar('zBiasL', 1.35) 
setvar('zBiasR', 4.5)
setvar('yBiasL', 1.4)
setvar('yBiasR', 3.4) 
setvar('xBiasL', -4.8) 
setvar('xBiasR', 1.3)
setvar('topBias1', 0.2)
setvar('topBias2', 0)

setvar('rfOn', False)


#setvar('desc', "5*s MOT --> 180 MHz CMOT, 8A, snap to 40A,  image on F=2, d=5 MHz, 300A, evaporate:  (5*s, 150->100), (5*s, 100->50)")

setvar('desc', "5*s MOT --> 180 MHz CMOT, 8A, snap to 40A,  image on F=2, d=5 MHz")

setvar('probeFrequency', 174)

# Global definitions


t0 = 200*ms

event(ch(digitalOut, 22), t0, 1)
event(ch(digitalOut, 22), t0 + 1*s, 0)

setvar('deltaImagingFreqSequence', 10e+06)
setvar('deltaImagingFreq', deltaImagingFreqSequence)
event(imagingOffsetFrequency, t0, 1.8e+09 + deltaImagingFreq)

event(probeLightRFSwitch, t0, probeLightOff)             # AOM is off, so no imaging ligh
event(probeLightShutter, t0+1*ms, 0)

event(ddsRfKnife, t0 + 1*ms, (180, 0, 0))
event(sixPointEightGHzSwitch, t0, 0)

event(capacitorPreChargeRelay, t0 - 1*ms, 5)
event(capacitorPreChargeRelay, t0 + 2.5*s, 0)
event(preChargeVoltage, t0-10*us, 1) # set cap voltage to 45V using Lambda

setQuadrupoleCurrent(t0 + 5*ms, 8, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 0)

# digital trigger
event(ch(digitalOut, 4), t0 + 2*s - 500*us, 1)
event(ch(digitalOut, 4), t0 + 2*s + 1*ms, 0)

#### Make a mot ####
time = t0 + 3*s


setvar('chargeTime', 15*ms)



# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)

setQuadrupoleCurrent(time, 35, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 8, rampRate = 3)

setQuadrupoleCurrent(time + 40*ms, 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 35)

if (usePreCharge) :
    setvar('preChargeCurrent', 40)
    preChargeTime = time - 5*ms
    event(quadrupoleChargeSwitch, preChargeTime, 1)
#    setQuadrupoleCurrent(preChargeTime + chargeTime - 1*ms, preChargeCurrent, applyCurrentRamp = False, usePrecharge = False, startingCurrent = 0)
    event(quadrupoleChargeSwitch, preChargeTime + chargeTime, 0) # was 5*ms before we started monkeying with it (i.e. for 45A fast turn on)
    time = preChargeTime + chargeTime







