from timing import *
#from numpy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''')

slowAnalogOut = dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('FPGA Fast Analog Out', 'ep-timing1.stanford.edu', 6)
trigger = dev('FPGA Trigger', 'ep-timing1.stanford.edu', 8)

setvar('signal0',     ch(fastAnalogOut, 0)) # The only input channel right now


# Define different blocks of the experiment
def MOT(Start):
    event(ch(trigger, 0), 1*ns, "Stop" )
    event(ch(trigger, 0), 2*ns, "Pause" )
    event(ch(trigger, 0), 3*ns, "Play" )
   
    # for i in range(1,1023) :

    #for i in range(1,100) :
    #    event(signal0, i*0.01*s, 1)
    #    event(signal0, i*0.01*s+0.005*s, -1)

    t0 = 0.1*s

    event(ch(fastAnalogOut, 0), (t0 +(4*us)+5*us), 11 )
    event(signal0, 0.1*s, 0)
    event(signal0, 1*s, 5)
    event(signal0, 2*s, 0)
   
    for i in range(1, 100) :
        event(ch(slowAnalogOut, 0), (t0 + i*(4*us)), 0 )
        event(ch(slowAnalogOut, 0), (t0 + i*(4*us)+2*us), 5 )

    event(ch(fastAnalogOut, 0), (t0 +(4*us)+5*us), 13 )

    #event(ch(trigger, 0), 10*s, "Stop" )

    return Start


# Global definitions


time = 0
time = MOT(time)
