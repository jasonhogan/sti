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

#setvar('imageCropVector',(570, 500, 300))
#setvar('imageCropVector',(500, 500, 300))
setvar('imageCropVector',(500, 500, 490))

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 0.1*ms)


setvar('MOTLoadTime', 1*s )

setvar('opticallyPump', True)

setvar('vcaVoltage', 0)
setvar('timeNoRepump', 20*ms)
setvar('dtOpticalPumping', 100*us)
setvar('cmotPower', 0.71)

setvar('desc', "CMOT, 100*us depump pulse, imaging on F=2, probe light @ 25% for 25*us, F=185 MHz, with shutter on TA2 input light, scan raman pulse, with TA5, with rf, with atom filter")


# Global definitions

t0 = 200*ms

event(TA4, t0 + 10*us, 0)
event(TA5, t0 + 10*us, 0)
event(TA6, t0 + 10*us, 0)

event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light
event(motLightShutter, t0 + 100*us, 1)                                  # open ta2 mot light Mechanical Shutter

#### Make a mot ####
time = t0
setvar('varCMOTCurrent', 0)

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 5*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 4, dtCMOT = 21*ms, powerReduction = cmotPower, CMOTFrequency = 210, dtNoRepump = timeNoRepump, repumpAttenuatorVoltage = vcaVoltage, cmotCurrentRampRate = 1.0)

    
tOff = time
setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
event(quadrupoleOnSwitch, tOff, 0)
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)



#### Drift ###
time = time + dtDriftTime

#### repump out of F = 1' #####
if (opticallyPump) :
    time = repumpMOT(time + 10*us, pumpingTime = 1000*us)

##Image

dtDeadMOT = 100*ms


### Andor Camera ###
andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
camera = ch(andorCamera, 0)
takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)


      
## Turn on MOT steady state

time = MOT(time + dtDeadMOT + 200*ms, leaveOn=True, cMOT = False, motQuadCoilCurrent = 8)    # turn MOT back on
event(motLightShutter, time + 100*us, 1)                                  # open ta2 mot light Mechanical Shutter










