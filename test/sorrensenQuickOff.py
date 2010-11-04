from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Turn TAs Off.''')

include("channels.py")

# Define different blocks of the experiment
def turnSFAOff(Start):
   

    #Initialization Settings
    tStart =1*ms

    ## Settings ##
    tOff =  tStart + 100*ms 

    event(ch(digitalOut, 4), tOff - 10*ms, 0)
    event(ch(digitalOut, 4), tOff - 100*us, 1)
    event(ch(digitalOut, 4), tOff + 100*ms, 0)



    setQuadrupoleCurrent(tOff-0.1*ms, 0, False, False)
    event(sfaOutputEnableSwitch, tOff - 0.1*ms, 0)
    event(quadrupoleOnSwitch, tOff, 0)
  

    event(quadrupoleOnSwitch, tOff + 10*ms, 1)    

    event(sfaOutputEnableSwitch, tOff + 11*ms, 1)

    commandTime = tOff + 25*ms


    setQuadrupoleCurrent(startTime = commandTime + 10*us, desiredCurrent = 55, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 0)


    return Start


# Global definitions

t0 = 10*us

time = t0
time = turnSFAOff(time)
