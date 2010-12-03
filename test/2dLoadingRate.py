from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Kill 3D every 500ms to test 2D Loading rate.''')

fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)

TA2 = ch(fastAnalogOut, 0)

# Define different blocks of the experiment
def MOT(Start):

    #Initialization Settings
    tStart = Start
    tWait = 1*ms
    
    ## TA Settings ##
    voltageTA = 1.6
    tLoseAtoms = 500*ms
    tLoad = 1000*ms

    #################### events #######################

#    event(ch(trigger, 0), 10*us, "Stop" )
#    event(ch(trigger, 0), 30*us, "Play" )

    dt = tLoad

    for i in range(0,10) :    
        # digital trigger
        event(ch(digitalOut, 4), tStart + i*tLoad, 1)
        event(ch(digitalOut, 4), tStart + (i)*tLoad+tLoseAtoms, 0)
        event(TA2, tStart + tLoad * i, 0)     # TA on
        event(TA2, tStart + tLoseAtoms + tLoad * i, voltageTA)     # TA off
  
    event(TA2, tStart + tLoad * (i+1), voltageTA)     # TA on

    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
