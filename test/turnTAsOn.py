from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Turn TAs On.''')

include("channels.py")

# Define different blocks of the experiment
def turnTAsOn(Start):
   

    #Initialization Settings
    tStart =1*ms

    ## TA Settings ##
#    voltageTA2 = 1.6
#    voltageTA3 = 1.5
    tTAOn = tStart + 10*ms


    event(TA2, tTAOn, voltageTA2)    # TA 2 On
    event(TA3, tTAOn, voltageTA3)    # TA 3 On
    event(TA7, tTAOn, ta7MotVoltage)    # TA 7 On  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = turnTAsOn(time)
