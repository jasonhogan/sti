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
    tOff =  tStart + 10*ms 


    setQuadrupoleCurrent(tOff-0.1*ms, 0, False, False)
    event(sfaOutputEnableSwitch, tOff-0.1*ms, 0)
    event(quadrupoleOnSwitch, tOff, 0)
  

    event(quadrupoleOnSwitch, tOff + 15*ms, 1)    

    event(sfaOutputEnableSwitch, tOff + 16*ms, 1)

    commandTime = tOff + 55*ms
#    event(sfaOutputEnableSwitch, commandTime - 10*us, 0)
#    event(quadrupoleChargeSwitch, commandTime, 1)
#    event(sfaOutputEnableSwitch, commandTime + 10*us, 1)

    setQuadrupoleCurrent(commandTime + 500*us, 25, True, False)

#    setQuadrupoleCurrent(commandTime + 2500*us, 0, True, False)


#    event(quadrupoleChargeSwitch, commandTime + 20*ms, 0)

#    setQuadrupoleCurrent(tOff + 23*ms, 18)

#    setQuadrupoleCurrent(tOff + 31*ms, 28)

#    setQuadrupoleCurrent(tOff + 39*ms, 38)

#    setQuadrupoleCurrent(tOff + 187*ms, 48, True)

#    event(quadCoilVoltage, tOff + 15*ms, 0.18)
#    event(quadCoilVoltage, tOff + 16*ms, 0.22)
#    event(quadCoilVoltage, tOff + 17*ms, 0.26)
#    event(quadCoilVoltage, tOff + 18*ms, 0.30)
#    event(quadCoilVoltage, tOff + 19*ms, 0.34)
#    event(quadCoilVoltage, tOff + 20*ms, 0.38)
#    event(quadCoilVoltage, tOff + 21*ms, 0.42)
#    event(quadCoilVoltage, tOff + 22*ms, 0.46)
#    event(quadCoilVoltage, tOff + 23*ms, 0.5)
#    event(quadCoilVoltage, tOff + 33*ms, 0.8)

    return Start


# Global definitions

t0 = 10*us

time = t0
time = turnSFAOff(time)
