from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

include('channels.py')
include('constants.py')
include('motFunction.py')
include('andorCameraFunctions.py')
include('evaporativeCoolingFunction.py')
include('repumpFunction.py')
include('depumpFunction.py')
include('setInitialConditions.py')


setvar('imageCropVector',(500, 500, 490))

setvar('dtDriftTimeSequence', 1000*us)
setvar('dtDriftTime', dtDriftTimeSequence)
#setvar('dtDriftTime', 10*ms)

setvar('MOTLoadTime', 1*s )

setvar('rapidOffSetVar', True)

setvar('realTime', False)


#setvar('desc', "molasses - with pull back to 180 MHz for 20*ms, with repump, 100*us depump pulse - pull power back to 100% TA command - 0.3*ms rabi pulse, scan from 20 to 220kHz in steps of 2kHz")

setvar('desc', "molasses hold - 1*s load - 0 MHz detuned - 8A CMOT, 210 MHz for 20*ms @ 70% power, 0*ms with no repump, 1*ms sweep, 0*ms optical pumping, molasses hold for 1*ms, drift sequence to 40*ms")

### Absorption Imaging Beam Frequency ###
setvar('probeFrequency', 224)

# Global definitions


t0 = 2*ms

time = setInitialConditions(t0)


#### Make a mot ####

setvar('varCMOTCurrent', 8)
setvar('varCMOTFrequency', 210)

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 1*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = rapidOffSetVar, motQuadCoilCurrent = 8, dtCMOT = 20*ms, powerReduction = 0.7, CMOTFrequency = varCMOTFrequency, dtNoRepump = 0*ms, repumpAttenuatorVoltage = 0)

#### digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)

### Molasses Hold ###
setvar('molassesDetuning', 20)
event(ch(dds,1), time + 10*us, (ddsMotFrequency + molassesDetuning, 100, 0) )

setvar('dtDriftBeforeMolasses', 100*us)
time = time + dtDriftBeforeMolasses

setvar('ta7MolassesVoltage', ta7MotVoltage)

event(TA2, time, voltageTA2)                     # TA2 on
event(TA3, time, 0)                                   # TA3 off - we only want vertical light
event(TA7, time, ta7MolassesVoltage)      # TA7 on - we want vertical light

setvar('dtMolassesHoldTime', 1*ms)
time = time + dtMolassesHoldTime

time = turnMOTLightOff(time)



#### Drift ###
time = time + dtDriftTime

##Image
dtDeadMOT = 100*ms

if(realTime) : 
    ### Take an absorbtion image using Andor Solis Software ###
    time = takeSolisSoftwareAbsorptionImage (time, 0.1*ms)
    
else : 
    ### Andor Camera ###
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    time = takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)

### End Of Sequence Functions ###
### Turn on MOT steady state ###

tTAEndOfSequence = time +2*ms
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False)    # turn MOT back on

#event(ch(digitalOut, 4), time + 4*s, 0)








