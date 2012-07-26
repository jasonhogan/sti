from stipy import *

include('channels.py')
include('experimentalParameters.py')

ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Kill 3D every 500ms to test 2D Loading rate.''')
#
#slowAnalogOut = dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
#trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
#digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)

#TA3 = ch(slowAnalogOut, 14) #defined in channels now, but include not used
#TA8 = ch(slowAnalogOut, 9)

# Define different blocks of the experiment
def MOT(Start):

    #Initialization Setting
    tStart = Start+1000*ms
    tWait = 1*ms
    
    ## TA Settings ##
#    voltageTA = voltageTA3   #was hard coded 1.55 AFS 05/02/11
    tLoseAtoms = 100*ms
    tLoad = 500*ms

    #################### events #######################

#    event(ch(trigger, 0), 10*us, "Stop" )
#    event(ch(trigger, 0), 30*us, "Play" )

    for i in range(0,30) :    
        # digital trigger
        event(digitalSynch, tStart + i*tLoad, 1)
        event(digitalSynch, tStart + (i+0.5)*tLoad, 0)
#        event(TA3, tStart + (tLoad) * (i), 0)     # TA on
#        event(TA8, tStart + (tLoad) * (i)+10*us, 0)
        event(TA4, tStart + (tLoad) * (i)+10*us, 0)
#        event(TA3, tStart + tLoad * (i+0.5), voltageTA3)     # TA off
#        event(TA8, tStart + tLoad * (i+0.5)+10*us, voltageTA8)
        event(TA4, tStart + tLoad * (i+0.5)+10*us, ta4MotVoltage)

#    event(TA3, tStart + tLoad * (i+1), voltageTA3)     # TA on
#    event(TA8, tStart + tLoad * (i+1)+10*us, voltageTA8)
#    event(TA4, tStart + tLoad * (i+1)+10*us, ta4MotVoltage)

    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
