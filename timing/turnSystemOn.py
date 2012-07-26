from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Turn TAs On.''')

include("channels.py")
include("experimentalParameters.py")

# Define different blocks of the experiment
def turnTAsOn(Start):
   

    #Initialization Settings
    tStart =1*ms

    #Turn on Master Vortex
#    event(ch(masterVortex,2), tStart, 'On')

    ## TA Settings ##
    tTAOn = tStart #+ 15*s


    event(TA1, tTAOn, voltageTA1ConsCurr)    # TA 1 on
    event(TA2, tTAOn + 1*ms, voltageTA2ConsCurr)    # TA 2 on 
    event(TA3, tTAOn + 2*ms, voltageTA3ConsCurr)    # TA 3 on
    event(TA8, tTAOn + 2.25*ms, voltageTA8ConsCurr)    # TA 7 on
    event(TA4, tTAOn + 3*ms, ta4MotVoltageConsCurr)    # TA 4 on
    event(TA5, tTAOn + 4*ms, 0)    # TA 5 off
    event(TA6, tTAOn + 5*ms, 0)    # TA 6 off
    event(TA7, tTAOn + 2.5*ms, ta7MotVoltageConsCurr)    # TA 7 on

    return Start


# Global definitions

t0 = 10*us

time = t0
time = turnTAsOn(time)

print "Good Morning!"
