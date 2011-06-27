from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Turn TAs Off.''')

include("channels.py")

# Define different blocks of the experiment
def turnTAsOff(Start):
   

    #Initialization Settings
    tStart =1*ms

    ## TA Settings ##
    tTAOff =  tStart + 10*ms 


    event(TA1, tTAOff, 0)    # TA 1 off
    event(TA2, tTAOff + 1*ms, 0)    # TA 2 off 
    event(TA3, tTAOff + 2*ms, 0)    # TA 3 off
    event(TA4, tTAOff + 3*ms, 0)    # TA 4 off
    event(TA5, tTAOff + 4*ms, 0)    # TA 5 off
    event(TA6, tTAOff + 5*ms, 0)    # TA 6 off
    event(TA7, tTAOff + 2.5*ms, 0)    # TA 7 off
  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = turnTAsOff(time)
