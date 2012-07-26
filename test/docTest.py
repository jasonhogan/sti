from stipy import *
#from numpy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0


#include('testInclude.py')
#include('includes/testInclude2.py')

# Set description used by program
#setvar('desc','''Test experiment.''')
#
digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
#slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
#dds = dev('DDS_ch0', 'ep-timing1.stanford.edu', 0)

#setvar('signal0',     ch(fastAnalogOut, 0)) # The only input channel right now


# Define different blocks of the experiment
def MOT(Start):
    
#    event(ch(trigger, 0), 10*us, "Stop" )
#    event(ch(trigger, 0), 20*us, "Play" )

    setvar('test', 55)

#    event(ch(digitalOut,1), 10*ms, 1)
#    event(ch(digitalOut,1), 100*ms, 0)
#
#    event(ch(digitalOut,1), 200*ms, 1)
#    event(ch(digitalOut,1), 250*ms, 0)
#
#    event(ch(fastAnalogOut,0), 10*ms, 0)
#    event(ch(fastAnalogOut,0), 100*ms, 0.3)
#    event(ch(fastAnalogOut,0), 200*ms, 0.6)
#    event(ch(fastAnalogOut,0), 300*ms, 0.9)
#
#    event(ch(fastAnalogOut,1), 10*ms, 0)
#    event(ch(fastAnalogOut,1), 150*ms, 0.5)

    return Start


# Global definitions


t0 = 10*us

time = t0
time = MOT(time)
