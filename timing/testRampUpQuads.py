from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

#setvar('cmotBoost',12)

include('channels.py')
include('experimentalParameters.py')
include('makeCMOTFunction.py')
include('repumpFunction.py')
include('depumpFunction.py')
include('andorCameraFunctions.py')

setvar('desc', "Ramp up quads")

rampUp = True
rampDown = True

time = 10*ms

if (rampUp) :
    time = rampUpQuadCoils(time, True, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, chargeCurrent = CMOTcurrent)

time += 5*s

if (rampDown) :
    time = rampDownQuadCoils(time, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, dischargeCurrent = varDischargeCurrent, rapidOff = False)

