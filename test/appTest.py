from stipy import *
#from numpy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0


include('testInclude.py')
include('includes/testInclude2.py')

# Set description used by program
setvar('desc','''Test experiment.''')
#
#digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
##slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
#fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
#trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
##dds = dev('DDS_ch0', 'ep-timing1.stanford.edu', 0)

appTest = dev('TestApp1', 'ep-timing1.stanford.edu', 0)

#setvar('signal0',     ch(fastAnalogOut, 0)) # The only input channel right now


# Define different blocks of the experiment
def MOT(Start):
    
#    event(ch(trigger, 0), 10*us, "Stop" )
#    event(ch(trigger, 0), 20*us, "Play" )
    setvar('test', 55)

    event(ch(appTest,1), 10*ms, 1)

    event(ch(appTest,1), 20*ms, 1.2)


    event(ch(appTest,1), 15*ms, 2)

    event(ch(appTest,1), 200*ms, 2.5)

    event(ch(appTest,1), 500*ms, 1.3)
    
    event(ch(appTest,1), 50*s, 4.3)
    return Start


# Global definitions


t0 = 10*us

time = t0
time = MOT(time)
