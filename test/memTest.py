from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''') 

#trigger = dev('FPGA_Trigger', 'timing-test.stanford.edu', 8)
#dds = dev('DDS', 'timing-test.stanford.edu', 7)


#digitalOut = dev('Digital Out', 'timing-test.stanford.edu',2)
#fastAnalogOut=dev('Fast Analog Out', 'timing-test.stanford.edu', 6)

trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
#digitalOut = dev('Digital Out', 'ep-timing1.stanford.edu',2)
#fastAnalogOut=dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)

setvar('ddsFreq',1)


# Define different blocks of the experiment
def MOT(Start):

    for i in range(1, 5) :
        event(ch(dds, 2), Start+i*1*ms, (1.0+0.01*i, 100, 0))

    event(ch(dds, 2), Start+(i+1)*1*ms, (1.0+0.01*i, 1000, 0))

    return Start


# Global definitions

 
t0 = 10*us

time = t0
time = MOT(time+100*us)
