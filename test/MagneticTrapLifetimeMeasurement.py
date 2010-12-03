from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

include('channels.py')
include('motFunction.py')
include('absorptionImageFunction.py')
include('evaporativeCoolingFunction.py')
include('repumpFunction.py')
include('depumpFunction.py')

#setvar('imageCropVector',(551, 375 ,250))
setvar('imageCropVector',(500, 500, 490))

setvar('dtDriftTimeSequence', 1000*us)
setvar('dtDriftTime', dtDriftTimeSequence)
#setvar('dtDriftTime', 0.4000*ms)

setvar('holdTimeSequence', 1*s)
setvar('holdTime', holdTimeSequence)
#setvar('holdTime', 30*s)

setvar('MOTLoadTime', 4500*ms )
setvar('magneticTrap', True)

setvar('xBias', 3.4)
setvar('yBias', 0.2)
setvar('zBias', 0.87)


setvar('desc',"mag trap loading - lifetime at 255 Amps")

# Global definitions


t0 = 2*ms


event(starkShiftingAOM, 100*us, starkShiftOff)
event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light


#### Make a mot ####
time = t0

time = MOT(time, tClearTime=100*ms, cMOT=True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT=1*ms, cmotQuadCoilCurrent = 45, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 7, dtCMOT = 40*ms, powerReduction = 1, CMOTFrequency = 180, dtNoRepump = 0*ms)

time = depumpMOT(time + 1*us, pumpingTime = 100*us)

# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)


#

if (magneticTrap) :
    setvar('fullMagneticTrapCurrent',255)
    setvar('chargeCurrent', 45)
    setvar('dischargeCurrent', 45)

    topCoilCurrentSetpoint=ch(slowAnalogOut,3)
    
#    event(quadrupoleChargeSwitch, time, 1)

    setQuadrupoleCurrent(time + 500*us, chargeCurrent, False, False, 0)
    event(quadrupoleChargeSwitch, time + 5*ms, 0)

    time = setQuadrupoleCurrent(time + 1*ms, fullMagneticTrapCurrent, True, False, chargeCurrent)

#    event(topCoilCurrentSetpoint, time + 33*us, 6)

    time = time + holdTime

#    event(topCoilCurrentSetpoint, time, 0)

    time = setQuadrupoleCurrent(time + 1*ms, dischargeCurrent, True, False, fullMagneticTrapCurrent)

#    endOfChargeTime = time + 99.1*s
#    event(ch(digitalOut, 4), endOfChargeTime - 500*us, 1)
#    event(ch(digitalOut, 4), endOfChargeTime + 1*ms, 0)

   
else :
    time = time + 1*ms


tOff = time
setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
event(quadrupoleOnSwitch, tOff, 0)


#time = evaporate(time, 125*ms, magneticTrapSetpoint = 125, rapidOff = False, cmotCurrent = 25)

#### Drift ###
time = time + dtDriftTime

#### repump out of F = 1' #####
time = repumpMOT(time, pumpingTime = 150*us)



#setQuadrupoleCurrent(time, desiredCurrent = 0, applyCurrentRamp = False, usePrecharge = False, startingCurrent = 0)
#event(quadrupoleOnSwitch, time, 1)



##Image
dtDeadMOT = 100*ms

setvar('realTime', False)

if(realTime) : 
     ## Take an absorbtion image using Andor Solis Software ##
    time = takeSolisSoftwareAbsorptionImage (time)
    
else : 
#    meas(absoptionLightFrequency, t0)
    ### Andor Camera ###
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    print time
    time = takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)


    ## Turn on MOT steady state

tTAEndOfSequence = time +2*ms
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False)    # turn MOT back on

event(ch(digitalOut, 4), time + 20*s, 0)








