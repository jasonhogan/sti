from stipy import *

include('approximateExponentialSweep.py') 

ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Debugging the DDS sweep function to add arbitrary waveform support.''') 


dds = dev('DDS', 'ep-timing1.stanford.edu', 0)

setvar('ddsFreq',1)


# Define different blocks of the experiment
def MOT(Start):

    event(ch(dds, 3), 1*ms, ((68,78,5*s), 25, 0) )
    event(ch(dds, 3), 6*s, (68, 25, 0) )
    
#    event(ch(dds, 2), 1*ms, (180, 100, 0) )

#    event(ch(dds,2), 100*ms, ((21, 51, 1900*ms), 100, 0))
#    event(ch(dds, 2), 1*ms + 4*s, ((91,31, 5*s), 100, 0) )

#    event(ch(dds, 2), 4*s, (31, 100, 0) )

#    event(ch(dds, 2), 4.1*s, ((30,31,0.2*s), 100, 0) )

#    event(ch(dds, 2), 4.5*s, (31, 100, 0) )

#    event(ch(dds, 2), 5*s, ((31,41,2*s), 100, 0) )

#    event(ch(dds, 2), 0.1*s, (11, 100, 0) )

#    event(ch(dds, 2), 1*s, (((11, 41, 7*s), (41,51, 0.1*s), (51, 61, 1*s), (61, 71, 5*s), (71, 85, 3*s)), 100, 0) )

#    event(ch(dds, 2), 3*s, (((91, 71, 2*s), (71,61, 3*s), (61, 31, 1*s)), 100, 0) )

#    event(ch(dds,2), 3*s, ([(50, 37.4, 1*s), (37.4, 30, 1*s), (30, 25.8, 1*s), (25.8, 23.4, 1*s), (23.4, 21.9, 1*s), (21.9, 21.14, 1*s)], 100, 0))
#    event(ch(dds,2), 10*ms, (approximateExponentialSweep(dt = 2*s, fStart = 180, fStop = 130, numberOfSteps = 5, tcFactor = 1), 100, 0))
#    event(ch(dds,2), 11*ms + 2*s, (approximateExponentialSweep(dt = 1*s, fStart = 130, fStop = 105, numberOfSteps = 10, tcFactor = 1), 100, 0))
#    event(ch(dds,2), 11.5*ms + 3*s, (approximateExponentialSweep(dt = 0.5*s, fStart = 105, fStop = 97, numberOfSteps = 10, tcFactor = 1), 100, 0))

#    event(ch(dds, 2), 5*s, ((91,41,10*s), 100, 0) )

#    event(ch(dds, 2), 28*s, (((1,1,1*s) ,(1,2,3)), 100, 0) )

#    event(ch(dds, 2), 11*s, (51, 0, 0) )






    tDDS=Start+5*s

    return Start


# Global definitions

 
t0 = 10*us

time = t0
time = MOT(time)
