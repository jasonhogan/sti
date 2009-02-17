from timing import *

ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''')


analogIn = dev('analog_in_ch3', 'ep-timing1.stanford.edu', 3)
vco0 = dev('ADF4360-0',   '128.12.174.77', 0)
vco1 = dev('ADF4360-5',   '128.12.174.77', 1)
vco2 = dev('ADF4360-5',   '128.12.174.77', 2)
vco3 = dev('ADF4360-6',   '128.12.174.77', 3)

setvar('temperature',     ch(analogIn, 0)) # The only input channel right now
setvar('lockRF',  ch(vco2, 0)) 	                  #channel 0 is frequency in the VCO


# Define different blocks of the experiment
def MOT(Start):

    test = [8];
    for i in range(1,5) :
     test.append(i)

    event(lockRF, 1.0*s, 1500)
    event(ch(vco0, 0), 2.0*s, 1500)
    event(lockRF, 3.0*s, 1600)
    event(ch(vco0, 1),     4.0*s,   "Off")
    event(ch(vco0, 1),     6.0*s,   "Off")
    event(ch(vco0, 1),     6.0*s+ns,   "On")
   # event(ch(vco0, 1),     25.0*s,   "Off")
   # event(ch(vco0, 1),     6.1*s,   "On")
   # event(ch(analogIn, 0),     2.0*s,   0)

    return Start


# Global definitions


time = 0
time = MOT(time)
