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


setvar('zBiasL', 4.20) 
setvar('zBiasR', 5.48)
setvar('yBiasL', 0.0)
setvar('yBiasR', 4.351) 
setvar('xBiasL', -2.55) 
setvar('xBiasR', 2.12)
setvar('topBias1', 0.06)
setvar('topBias2', -0.05)
setvar('quadrupoleCompensation', -7.61)



setvar('desc', "Look at current decay rate with 0 Amps commanded from 40 Amps")


# Global definitions


t0 = 2*ms

time = t0

setvar('quadCoilCurrent', 300)


# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)

time = setQuadrupoleCurrent(time, desiredCurrent = quadCoilCurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 0, rampRate = 1)

time = time + 20*ms
tOff = time + 51*ms

setQuadrupoleCurrent(time, desiredCurrent = 40, applyCurrentRamp = False, usePrecharge = False, startingCurrent = quadCoilCurrent, rampRate = 1)


setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
event(quadrupoleOnSwitch, tOff, 0)









