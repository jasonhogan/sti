from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test the uniblitz shutter''')

include("channels.py")
include("bluePlugShutterFunction.py")


t0 = 2*ms
time = t0 + 1000*ms


event(ch(digitalOut, 4), time - 1*ms, 0)
event(ch(digitalOut, 4), time, 1)
event(ch(digitalOut, 4), time + 10*us, 0)

#turn off test
#openBluePlugShutter(time-500*ms)
#closeBluePlugShutter(time)

#turn on test
openBluePlugShutter(time)
closeBluePlugShutter(time + 500*ms)