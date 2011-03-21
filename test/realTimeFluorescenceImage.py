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
include('repumpFunction.py')
include('depumpFunction.py')

setvar('desc',"MOT lifetime")
setvar('imageCropVector',(500, 500, 490))
setvar('dtFluorescenceExposure',10*ms)

time = 10*ms

time = MOT(time, tClearTime=100*ms, cMOT=True, dtMOTLoad=1*s, dtSweepToCMOT = 1*ms, cmotQuadCoilCurrent = 8, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 8, dtCMOT = 10*ms, powerReduction = 1.0, CMOTFrequency = 140, dtNoRepump = 0*us, repumpAttenuatorVoltage = 0)

#time = MOT(time, leaveOn=True, cMOT = False)    # turn MOT back on
#time = time + 1*s
#
#turnMOTLightOff(time)
time = time + 10*us
#turnMOTLightOn(time)
time = time + 1*us

#### repump out of F = 1' #####
#time = repumpMOT(time + 10*us, pumpingTime = 1*ms)

## Take an Fluorescence image using Andor Device ##    

time = takeSolisSoftwareFluorescenceImage(time, dtFluorescenceExposure = 10*ms, leaveMOTLightOn = False)

time = time + 100*ms

time = MOT(time, leaveOn=True, cMOT = False)    # turn MOT back on