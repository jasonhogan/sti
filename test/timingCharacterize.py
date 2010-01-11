from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Characterize Timing System.''')

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
#vco1=dev('ADF4360-5', 'ep-timing1.stanford.edu', 1)
trigger=dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)

# Define different blocks of the experiment
def MOT(Start):

    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 0), 20*us, "Pause" )
    event(ch(trigger, 0), 30*us, "Play" )

    event(ch(digitalOut, 0),1*ms, 1 )
    event(ch(digitalOut, 0), 2*ms, 0 )
    event(ch(digitalOut, 0), 3*ms, 1 )
    event(ch(digitalOut, 0), 4*ms, 0 )

    tS = 10*ms
    dT = 500*ns

    event(ch(digitalOut,1), tS, 1)
#    event(ch(digitalOut,1), tS+dT, 0)
#    event(ch(digitalOut,1), tS+2*dT, 1)
    event(ch(digitalOut,1), tS+3*dT, 0)

    dCh = 500*ns

    event(ch(digitalOut,2), tS+dCh, 1)
    event(ch(digitalOut,2), tS+dCh+10*dT, 0)

    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
