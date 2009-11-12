from stipy import *
#from numpy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''')

slowAnalogOut = dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('FPGA Fast Analog Out', 'ep-timing1.stanford.edu', 6)
#trigger = dev('FPGA Trigger', 'ep-timing1.stanford.edu', 8)

#setvar('signal0',     ch(fastAnalogOut, 0)) # The only input channel right now


# Define different blocks of the experiment
def MOT(Start):
#    event(ch(trigger, 0), 1*us, "Stop" )
#    event(ch(trigger, 0), 2*us, "Pause" )
#    event(ch(trigger, 0), 3*us, "Play" )
   
    # for i in range(1,1023) :

    #for i in range(1,100) :
    #    event(signal0, i*0.01*s, 1)
    #    event(signal0, i*0.01*s+0.005*s, -1)

    t0 = 0*s
   
    for i in range(1, 100) :
        event(ch(slowAnalogOut, 0), (t0 + i*(150*us)), -10 )
        event(ch(slowAnalogOut, 0), (t0 + i*(150*us)+75*us), 10 )

    for i in range(1, 100) :
        event(ch(slowAnalogOut, 1), (t0 + i*(150*us)+10*us), -10 )
        event(ch(slowAnalogOut, 1), (t0 + i*(150*us)+85*us), 10 )

    for i in range(1, 100) :
        event(ch(fastAnalogOut, 0), (t0 + i*(150*us)), -10 )
        event(ch(fastAnalogOut, 0), (t0 + i*(150*us)+75*us), 10 )

    for i in range(1, 100) :
        event(ch(fastAnalogOut, 1), (t0 + i*(150*us)+1.25*us), -10 )
        event(ch(fastAnalogOut, 1), (t0 + i*(150*us)+76.25*us), 10 )


    #event(ch(trigger, 0), 10*s, "Stop" )

    return Start


# Global definitions


time = 0
time = MOT(time)
