from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''') 


trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)

# Define different blocks of the experiment
def MOT(Start) :

#    event(ch(dds, 1), Start, (180, 100, 0) )

#    event(ch(dds,1), Start + 20*us, ((140,220,3*s), 100, 0) )

    event(ch(dds,1), Start +20*us, (140, 100, 0) )

    return Start


# Global definitions
 
t0 = 10*us

time = t0
time = MOT(time+100*us)
