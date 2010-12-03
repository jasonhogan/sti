from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Turn on absorption beam AOM steady state.''')

include("channels.py")

# Define different blocks of the experiment
def turnOnImagingLight(tStart):

    event(probeLightShutter, tStart, 0)                               #close probe light shutter
    event(probeLightAOM, tStart, probeLightOff)               #turn on absorbtion light
  
    return tStart


# Global definitions

t0 = 10*ms

turnOnImagingLight(t0)
