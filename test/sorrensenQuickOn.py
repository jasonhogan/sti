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
    tOn =  tStart + 10*ms 


    
    setQuadrupoleCurrent(tOn + 0.1*ms, 8, False, False)
    event(quadrupoleOnSwitch, tOn, 1)
    event(sfaOutputEnableSwitch, tOn + 100*us, 1)
        
    commandTime = tOn + 500*ms


    event(quadrupoleChargeSwitch, commandTime, 1)

#    event(sfaOutputEnableSwitch, commandTime - 500*us, 0)
#    event(sfaOutputEnableSwitch, commandTime + 100*us, 0)

    setQuadrupoleCurrent(commandTime + 500*us, 30, False, False, 0)

    event(ch(digitalOut, 4), commandTime - 100*ms, 0)
    event(ch(digitalOut, 4), commandTime - 100*us, 1)
    event(quadrupoleChargeSwitch, commandTime + 50*ms, 0)
    event(ch(digitalOut, 4), commandTime + 100*ms, 0)



    return Start


# Global definitions

t0 = 10*us

time = t0
time = turnSFAOff(time)
