from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

include('experimentalParameters.py')
include('channels.py')
include('makeCMOTFunction.py')
include('repumpFunction.py')
include('depumpFunction.py')
include('andorCameraFunctions.py')
include('bluePlugShutterFunction.py')
include('evaporativeCoolingFunctionLinear2withNetworkAnalyzer.py')

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', .1*ms)

#setvar('dtPlugOff', 50*ms)

#setvar('holdTimeSequence', 1*s)
#setvar('holdTimeAfterEvap', holdTimeSequence)
setvar('holdTimeBeforeEvap', 0*ms) #0
setvar('holdTimeAfterEvap', 0*ms) #0
setvar('numberOfRamps', 1)

setvar('desc', "Evaporation; " + str(MOTLoadTime/s) + " s load; Hold time before evaporation " + str(holdTimeBeforeEvap/s) + " s; Hold time after evaporation " + str(holdTimeAfterEvap/s) + " s; Drift time " + str(dtDriftTime/ms) + " ms; Evaporation Ramps: " + str(numberOfRamps))

###setvar('imageCropVector',(500, 500, 490))
setvar('imageCropVector',(1,60,1500,1500))
#setvar('imageCropVector',(554, 463, 25))

setvar('opticallyPump', True)
setvar('realTime', False)
setvar('opticallyPlugTrap', False)
setvar('insituImage', True)
setvar('highFieldImage', False)
setvar('depumpImage',False)


#setvar('repumpVCAseq',10)
#setvar('repumpVCA',repumpVCAseq)
setvar('repumpVCA', 1)

######## Prepare atom cloud ########

tStart = 100*us

event(imagingOffsetFrequency, tStart, imagingResonanceFreq + deltaImagingFreqMHz*1e6)

time = tStart
time = makeCMOT(time)
time = turnMOTLightOff(time)
time = depumpMOT(time + 10*us, pumpingTime = 200*us)

time += 50*ms #time for atoms to move from cMOT center to mag trap center at 35 G/cm

time = rampUpQuadCoils(time, True, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, chargeCurrent = CMOTcurrent, quadRampRate = 1)

time += holdTimeBeforeEvap

if(opticallyPlugTrap):
    openBluePlugShutter(time)
else:
    closeBluePlugShutter(time)


time = evaporate(time, rampNumber = numberOfRamps)

time += holdTimeAfterEvap

#####extra ramp down and up
#time = rampDownQuadCoils(time, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, dischargeCurrent = varDischargeCurrent, rapidOff = False,  quadRampRate = 1)
#time = rampUpQuadCoils(time, True, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, chargeCurrent = varDischargeCurrent, quadRampRate = 1)

highFieldImageTime=time-10*ms

#closeBluePlugShutter(highFieldImageTime - 5*ms)


#time = rampDownQuadCoils(time, fullMagneticTrapCurrent = 100, dischargeCurrent = varDischargeCurrent, rapidOff = False)

time = rampDownQuadCoils(time, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, dischargeCurrent = varDischargeCurrent, rapidOff = False,  quadRampRate = .250)


event(digitalSynch,time-10*ms,0) #trigger webscope for quad coil ramp
event(digitalSynch,time,1) 
event(digitalSynch,time+10*ms,0) 



## Wait for eddys to die down
time += 50*ms


#if(opticallyPlugTrap):
#    closeBluePlugShutter(time)

#openBluePlugShutter(time)
#time += 2*ms


##close shutter at high field
#closeBluePlugShutter(highFieldImageTime - dtPlugOff)

######## Measure the result: Snap off field, repump, and image ########

if(not insituImage) :
    #### Snap off magnetic field
#    time = rampDownQuadCoils(time, rapidOff = True)
#    time = rampDownQuadCoils(time, fullMagneticTrapCurrent = varDischargeCurrent, dischargeCurrent = 0, rapidOff = False,  quadRampRate = 2)
    event(sfaRemoteCurrentSetVoltage, time, 0)
    if(opticallyPlugTrap):
        closeBluePlugShutter(time)

if(not insituImage) :
    time += dtDriftTime

if(highFieldImage) :
    repumpTime = highFieldImageTime
else :
    repumpTime = time

### repump out of F = 1'
if (opticallyPump) :
    repumpTimeDone = repumpMOT(repumpTime + 10*us, pumpingTime = 1000*us)
    dtRepump = repumpTimeDone - repumpTime
else :
    dtRepump = 0

time += dtRepump

### Image

if(highFieldImage) :
    imageTime = highFieldImageTime +100*us + dtRepump
else:
    imageTime = time

### Snap off magnetic field before reference image
dtDeadMOT = 100*ms
if (insituImage):
 #### Snap off magnetic field
    time = rampDownQuadCoils(time + (dtDeadMOT)/10, rapidOff = True)
    


if(realTime) : 
    # Take an absorbtion image using Andor Solis Software
    imageDoneTime = takeSolisSoftwareAbsorptionImage(imageTime, expTime = 20*us, dtAbsorbtionLight = 10*us, iDus = True, depumpAbsImage = depumpImage)
#    imageDoneTime = takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, iXonImage = True, imagingDetuning = 0)

else : 
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    imageDoneTime = takeAbsorptionImage(imageTime, time + dtDeadMOT, cropVector=imageCropVector, depumpAbsImage = depumpImage)
#    imageDoneTime = takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 2*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 20)

time += (imageDoneTime - imageTime) 
###############################################################


if(opticallyPlugTrap and insituImage):
    closeBluePlugShutter(time+100*ms)


#closeBluePlugShutter(time+100*ms)

### Turn on MOT steady state ##################################################

MOT(time + 150*ms, leaveOn=True, cMOT = False)    # turn MOT back on


