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


    event(TA2, tTAOff, 0)    # TA 2 off 
    event(TA3, tTAOff, 0)    # TA 3 off
  
    return Start


# Global definitions

t0 = 10*us

time = t0
time = turnTAsOff(time)
