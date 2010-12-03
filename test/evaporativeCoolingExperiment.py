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

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 1*ms)

#setvar('holdTimeSequence', 1*s)
#setvar('holdTime', holdTimeSequence)
setvar('holdTime', 2*s)

setvar('rabiPulseTime', 100*us)

setvar('rbResonanceFreq', 6.834682610e+009 - 1e+003)
setvar('resFreq', rbResonanceFreq)

setvar('MOTLoadTime', 10*s )
setvar('magneticTrap', True)
setvar('opticallyPump', False)

setvar('xBias', 3.4)
setvar('yBias', 0.2)
setvar('zBias', 0.87)

setvar('desc',"MW Evaporation using new microwave horn position, and dds knife. rfCut: True; QuadCoil: 300 A; frequencyRamp: True; EvaporationTime: 1*s; 2e6 / 100ms; 180 to 160 MHz; 0.1*ms drift for TOF; 10*s in mag trap for lifetime")
setvar('probeFrequency', 195)

# Global definitions


t0 = 2*ms


event(starkShiftingAOM, 100*us, (50,0,0))
event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light
event(opticalPumpingBiasfield, t0 - 10*us, 0) # turn off optical pumping bias field
event(ddsRfKnife, t0 + 1*ms, (180, 0, 0))

#event(rfKnifeFrequency, t0, rbResonanceFreq - 2e+006)


#### Make a mot ####
time = t0

setvar('varCMOTCurrent', 7)

time = MOT(time, tClearTime=100*ms, cMOT=True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT=1*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 7, dtCMOT = 40*ms, powerReduction = 1.0, CMOTFrequency = 180, dtNoRepump = 3*ms)

if (opticallyPump) :
    event(opticalPumpingBiasfield, time - 1*ms, 6) # turn on TOP coils to apply a bias field along the optical pumping direction
    event(opticalPumpingBiasfield, time + 1000*us, 0) # turn off optical pumping bias field
#    event(probeLightAOM, time, (110, 100, 0))             # AOM on to begin optical pumping
    event(probeLightAOM, time + 100*us, probeLightOff)             # AOM on to begin optical pumping
    time = time + 100*us

#time = depumpMOT(time + 1*us, pumpingTime = 1500*us)
#
# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)


#

if (magneticTrap) :
    setvar('varFullMagneticTrapCurrent', 300)
    setvar('varChargeCurrent', 45)
    setvar('varDischargeCurrent', 45)
    time = evaporate(time, dtHold = holdTime, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, cmotCurrent = varCMOTCurrent, usePreCharge = True, chargeCurrent = varChargeCurrent, rapidOff = True, dischargeCurrent = varDischargeCurrent, makeRfCut = True)
    
else :
    tOff = time
    setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
    event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
    event(quadrupoleOnSwitch, tOff, 0)



#### Drift ###
time = time + dtDriftTime

#### repump out of F = 1' #####
time = repumpMOT(time + 10*us, pumpingTime = 1500*us)


#event(ddsRfKnife, time, (resFreq, 100, 0))

dtCut = 0.05*ms
#dtCut = rabiPulseTime

#event(sixPointEightGHzSwitch, time, 1)
#event(sixPointEightGHzSwitch, time + dtCut, 0)
#time = time + dtCut


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

#event(ch(digitalOut, 4), time + 4*s, 0)








