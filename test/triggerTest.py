from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''')

trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
#dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
digitalOut = dev('Digital Out', 'ep-timing1.stanford.edu',2)
fastAnalogOut=dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)


def MOT(Start):

#    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 8), 1*ns, "Wait" )
 
#    event(ch(digitalOut, 0), Start+50*ms, 0)
    event(ch(fastAnalogOut, 0), Start+50*ms, 0)
    event(ch(fastAnalogOut, 0), Start+100*ms, 5)

#    event(ch(trigger, 8), 1000*ms, "Play" )

    return Start


# Global definitions


t0 = 10*us

time = t0
time = MOT(time)
