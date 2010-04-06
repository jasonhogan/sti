from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Turn off 1530 light immediately before imaging.''')


trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)


aomSwitch0 = ch(dds, 0)


# Define different blocks of the experiment
def MOT(Start):

#    event(ch(trigger, 0), 10*us, "Stop" )
#    event(ch(trigger, 0), 30*us, "Play" )


    #AOM settings
#    absorptionFreq = 1067 
#    aomFreq0 = absorptionFreq / 8
    aomFreq0 = 110
    aomAmplitude0 = 100
    aomHoldOff = 10*us


    event(aomSwitch0, 10*ms, (aomFreq0, aomAmplitude0, 0)) #turn on absorbtion light
#    event(aomSwitch0, 10*ms, (aomFreq0, 0, 0)) #turn off absorbtion light

  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
