from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

include('channels.py')
include('motFunction.py')
include('absorptionImageFunction.py')
include('evaporativeCoolingFunction.py')
include('repumpFunction.py')
include('depumpFunction.py')

t0 = 2*ms

approximateExponentialSweep(tStart = t0, dt = 9*s, fStart = 50, fStop = 30, numberOfSteps = 5)

event(starkShiftingAOM, 10*s + t0, (50, 0, 0))