from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''') 


dds = dev('DDS', 'ep-timing1.stanford.edu', 0)

setvar('ddsFreq',1)


# Define different blocks of the experiment
def MOT(Start):

    event(ch(dds, 1), 1*ms, ((100,240,4*s), 100, 0) )



    tDDS=Start+55*us

    return Start


# Global definitions

 
t0 = 10*us

time = t0
time = MOT(time)
