from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

include('experimentalParameters.py')
include('channels.py')
include('repumpFunction.py')
include('depumpFunction.py')
include('andorCameraFunctions.py')
include('motFunction.py')
include('quadCoilControlCircuit.py')


setvar('desc', "Microwave Magnetometer")

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 15*ms)

setvar('imageCropVector',(500, 500, 490))

setvar('realTime', False)
setvar('microwaveRepump', True)

setvar('MOTLoadTime', 2.25*s )
setvar('MOTcurrent',8)    #8


#setvar('deltaFreqSeq', .15)
#setvar('deltaFreq', deltaFreqSeq)
setvar('deltaFreq', 0.55)
#setvar('dtRabiPulseTimeSeq', 100*us)
#setvar('dtRabiPulseTime', dtRabiPulseTimeSeq)
setvar('dtRabiPulseTime', 500*us)


#setvar('deltaImagingFreqMHz', -2.5)

######## Prepare atom cloud ########

tStart = 10*ms

event(imagingOffsetFrequency, tStart, imagingResonanceFreq + deltaImagingFreqMHz*1e6)

time = tStart




motOffTime = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 20*ms, cmotQuadCoilCurrent = 8, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = MOTcurrent, dtCMOT = 1*ms, powerReduction = 1.0, CMOTFrequency = 180, dtNoRepump = 5*ms, repumpAttenuatorVoltage = 0, cmotCurrentRampRate = 1)
time = motOffTime

### Snap off magnetic field
######time=rampDownQuadCoils(motOffTime -MOTLoadTime+10*us, rapidOff = True)
#rampDownQuadCoils(motOffTime - 50*ms, rapidOff = True)
time = snapOffField(time+10*us)

time = turn2DMOTLightOff(time)
time = turnMOTLightOff(time)



time += dtDriftTime

time = depumpMOT(time + 25*us, pumpingTime = 2000*us)

#### Microwave repump
if(microwaveRepump) :
#    event(ddsRfKnife, tStart-0.5*ms, (ddsRbResonanceFreq + deltaFreq, 100, 0))
    event(ddsRfKnife, tStart-0.5*ms, (ddsRbResonanceFreq + deltaFreq, ddsRbResonanceFreq + deltaFreq, .001))
    event(rfKnifeAmplitude, tStart-0.5*ms, 10)

    event(sixPointEightGHzSwitch, time, 1)
    event(sixPointEightGHzSwitch, time + dtRabiPulseTime, 0)
    time = time + dtRabiPulseTime



### Image


if(realTime) : 
    # Take an absorbtion image using Andor Solis Software
    time = takeSolisSoftwareAbsorptionImage (time, expTime = 75*us, dtAbsorbtionLight = 25*us, iDus = True)

else : 
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    imageTime = time
    dtDeadMOT = 500*ms
    time = takeAbsorptionImage(imageTime, imageTime + dtDeadMOT, cropVector=imageCropVector)
#    takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)

#    takeFluorescenceImage(imageTime, cropVector = imageCropVector)
#    takeFluorescenceImage(imageTime, dtFluorescenceExposure = 10*ms, cropVector = imageCropVector)
###############################################################
    

      
### Turn on MOT steady state ##################################################

MOT(time + 150*ms, leaveOn=True, cMOT = False)    # turn MOT back on


