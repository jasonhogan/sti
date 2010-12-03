from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test the uniblitz shutter''')

include("channels.py")
#include("bluePlugShutterFunction.py")


t0 = 2*ms
time = t0 + 1000*ms


event(ch(digitalOut, 4), time - 1*ms, 0)
event(ch(digitalOut, 4), time, 1)
event(ch(digitalOut, 4), time + 10*us, 0)

# TOP Coil pulse ON
event(opticalPumpingBiasfield, time-1*ms, 5.5)                 # turn on TOP coils to apply a bias field along the optical pumping direction
event(opticalPumpingBiasfield, time + 10*ms, 0)     # turn off optical pumping bias field
