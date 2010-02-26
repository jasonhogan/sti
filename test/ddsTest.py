from stipy import *
#from numpy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''')

#trigger = dev('FPGA_Trigger', 'timing-test.stanford.edu', 8)
#dds = dev('DDS', 'timing-test.stanford.edu', 7)
#digitalOut = dev('Digital Out', 'timing-test.stanford.edu',2)

trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 7)
#digitalOut = dev('Digital Out', 'ep-timing1.stanford.edu',2)


# Define different blocks of the experiment
def MOT(Start):
    event(ch(trigger, 0), 10*us, "Stop" )
    event(ch(trigger, 0), 20*us, "Play" )


    event(ch(dds, 0), 5*s, ( 35, 100, 0 ) )


#    event(ch(dds, 0), 10*s + 80*us, ( (150,25,5*s), 100, 0 ) )
#    event(ch(dds, 0), 4010*ms, ( (1,100,1000*ms), 100, 0 ) )
#    event(ch(dds, 0), 3*s, ( 35, 100, 0 ) )
#    event(ch(dds, 0), 10*ms + 40*us, (.035,100,0))
#    event(ch(digitalOut, 0), 7010*ms, 1)
#    event(ch(digitalOut, 0), 7010*ms + 40*us, 0)


    return Start


# Global definitions


t0 = 10*us

time = t0
time = MOT(time)
