from stipy import *

include('channels.py')
include('andorCameraFunctions.py')
include('makeCMOTFunction.py')

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

mot = ch(dds, 1)

# Define different blocks of the experiment
def MOT(Start):

    #Initialization Setting
    tStart = Start+1000*ms
    tWait = 1*ms
    
    ## TA Settings ##
    voltageTA = 1.55
    tLoseAtoms = 100*ms
    tLoad = 1000*ms

    #################### events #######################

#    event(ch(trigger, 0), 10*us, "Stop" )
#    event(ch(trigger, 0), 30*us, "Play" )

    for i in range(0, 48) :    
        # digital trigger
        event(digitalSynch, tStart + i*tLoad, 1)
        event(digitalSynch, tStart + (i+0.5)*tLoad, 0)
        event(mot, tStart + (tLoad) * (i), (150,100,0))     # TA on
        event(mot, tStart + tLoad * (i+0.5), (160,100,0))     # TA off
#        time=tStart + (tLoad) * (i)
#        takeSolisSoftwareAbsorptionImage (time, expTime = 75*us, dtAbsorbtionLight = 25*us, iDus = True)
#        time=tStart + tLoad * (i+0.5)
#        takeSolisSoftwareAbsorptionImage (time, expTime = 75*us, dtAbsorbtionLight = 25*us, iDus = True)

    event(TA3, tStart + tLoad * (i+1), voltageTA)     # TA on

    return Start


# Global definitions

t0 = 10*us

time = t0
time = MOT(time)
