#NOTE: Sound shutter is on

from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

include('channels.py')
include('experimentalParameters.py')
include('makeCMOTFunction.py')
include('repumpFunction.py')
include('depumpFunction.py')
include('andorCameraFunctions.py')
include('bluePlugShutterFunction.py')
include('evaporativeCoolingFunctionLinear2withNetworkAnalyzer.py')
include('motShutters.py')

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', .1*ms)

#setvar('dtPlugOff', 50*ms)

#setvar('holdTimeSequence', 1*s)
#setvar('holdTimeAfterEvap', holdTimeSequence)
setvar('holdTimeBeforeEvap', 0*ms) #0
setvar('holdTimeAfterEvap', 0*s) #0
setvar('dtHighFieldImageOffset', 0*ms) #0
setvar('holdTimeAfterRampToImaging', 0*ms) #100
setvar('numberOfRamps', 5)

setvar('soundAmplitude',0.55)
setvar('soundFrequencyHz', 1408)
setvar('activateSoundShutter', False)

setvar('dtRampToImaging', 1000*ms)#125*ms

#setvar('heatingHoldCurrent', 135)
#setvar('dtHeatingHoldTimeMidRamp',5000*ms)

setvar('desc', "Evaporation; " + str(MOTLoadTime/s) + " s load; Hold time before evaporation " + str(holdTimeBeforeEvap/s) + " s; Hold time after evaporation " + str(holdTimeAfterEvap/s) + " s; Drift time " + str(dtDriftTime/ms) + " ms; Evaporation Ramps: " + str(numberOfRamps))

###setvar('imageCropVector',(500, 500, 490))
#setvar('imageCropVector',(1,60,1500,1500))
#setvar('imageCropVector',(554, 463, 25))

setvar('imageCropVector',(1, 60+30, 1003-50, 913-50))

#setvar('opticallyPump', True)
setvar('realTime', False)
setvar('opticallyPlugTrap', True)
setvar('insituImage', False)
setvar('highFieldImage', False)
setvar('depumpImage',False)

opticallyPump, microwavePump, none = range(3)
repumpMethod = opticallyPump


#setvar('repumpVCAseq',10)
#setvar('repumpVCA',repumpVCAseq)
#setvar('repumpVCA', 1)

######## Prepare atom cloud ########

tStart = 100*us

event(imagingOffsetFrequency, tStart, imagingResonanceFreq + deltaImagingFreqMHz*1e6)

time = tStart
time = makeCMOT(time)
time = turnMOTLightOff(time)


time = depumpMOT(time + 200*us, pumpingTime = 100*us)  #200*us

#time += 50*ms #time for atoms to move from cMOT center to mag trap center at 35 G/cm

#triggerTime = time+0.5*ms
time = snapOnField(time+0.5*ms, snapCurrent = magTrapTransferCurrent)
sfaCurrent = magTrapTransferCurrent

#time += 50*ms

time = rampQuadrupoleCurrent(startTime = time, endTime = time+250*ms, startCurrent = magTrapTransferCurrent, endCurrent = varFullMagneticTrapCurrent, numberOfSteps = 150)
#time = rampUpQuadCoils(time, True, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, chargeCurrent = CMOTcurrent, quadRampRate = 0.5)

time += holdTimeBeforeEvap

if(opticallyPlugTrap):
    openBluePlugShutter(time)
else:
    closeBluePlugShutter(time)

if (numberOfRamps > 0) :
    closeMOTShutters(time)

intermediateRampRatio = (1.0*(varFullMagneticTrapCurrent - intermediateTrapCurrent) / (varFullMagneticTrapCurrent - imagingCurrent))

time = evaporate(time, rampNumber = numberOfRamps)

#setvar('dtIntermediateRamp', dtRampToImaging*intermediateRampRatio)
#setvar('tRampStart', time + 10*us)

#time += dtIntermediateRamp

### open sound shutter
if (activateSoundShutter):
    event(soundShutter, time - 100*ms, 1)

#closeBluePlugShutter(time)
time += holdTimeAfterEvap

### close sound shutter
if (activateSoundShutter):
    event(soundShutter, time -200*ms, 0)

#####extra ramp down and up
#time = rampDownQuadCoils(time, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, dischargeCurrent = varDischargeCurrent, rapidOff = False,  quadRampRate = 1)
#time = rampUpQuadCoils(time, True, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, chargeCurrent = varDischargeCurrent, quadRampRate = 1)

#time  = rampDownQuadCoils(time+1*ms, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, dischargeCurrent = 100, rapidOff = False, quadRampRate = 0.250)


highFieldImageTime=time + dtHighFieldImageOffset
#highFieldImageTime=time+10*ms
#time += 10*ms

if (highFieldImage):
    openMOTShutters(highFieldImageTime-5*ms)

#closeBluePlugShutter(highFieldImageTime - 5*ms)


#time = rampDownQuadCoils(time, fullMagneticTrapCurrent = 100, dischargeCurrent = varDischargeCurrent, rapidOff = False)

#time = rampDownQuadCoils(time, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, dischargeCurrent = varDischargeCurrent, rapidOff = False,  quadRampRate = .250)

if (numberOfRamps >=5) :
    rampQuadrupoleCurrent(startTime = tRampStart, endTime = tRampStart + dtIntermediateRamp, startCurrent = varFullMagneticTrapCurrent, endCurrent = intermediateTrapCurrent, numberOfSteps = 150*intermediateRampRatio)
    time = rampQuadrupoleCurrent(startTime = time, endTime = time+dtRampToImaging*(1 - intermediateRampRatio), startCurrent = intermediateTrapCurrent, endCurrent = imagingCurrent, numberOfSteps = 150*(1 - intermediateRampRatio)) #1*s and 150 steps
else:
    time = rampQuadrupoleCurrent(startTime = time, endTime = time+dtRampToImaging, startCurrent = varFullMagneticTrapCurrent, endCurrent = imagingCurrent, numberOfSteps = 150) #1*s and 150 steps
#    Comment the above, and uncomment the below to insert a 1s hold at an intermediate ramp current (test for parametric heating resonance)
#    if ((heatingHoldCurrent < imagingCurrent) or (heatingHoldCurrent > varFullMagneticTrapCurrent)):
#        print 1/0
#    dtRampToImagingBeforeHold = dtRampToImaging * (varFullMagneticTrapCurrent - heatingHoldCurrent)/(varFullMagneticTrapCurrent - imagingCurrent)
#    dtRampToImagingAfterHold = dtRampToImaging - dtRampToImagingBeforeHold
#    numberOfStepsFullRamp = 150 #150
#    numberOfStepsBeforeHold = numberOfStepsFullRamp * (varFullMagneticTrapCurrent - heatingHoldCurrent)/(varFullMagneticTrapCurrent - imagingCurrent)
#    numberOfStepsAfterHold = numberOfStepsFullRamp - numberOfStepsBeforeHold
#    time = rampQuadrupoleCurrent(startTime = time, endTime = time+dtRampToImagingBeforeHold, startCurrent = varFullMagneticTrapCurrent, endCurrent = heatingHoldCurrent, numberOfSteps = numberOfStepsBeforeHold) #1*s and 150 steps
#    time += dtHeatingHoldTimeMidRamp
#    time = rampQuadrupoleCurrent(startTime = time, endTime = time+dtRampToImagingAfterHold, startCurrent = heatingHoldCurrent, endCurrent = imagingCurrent, numberOfSteps = numberOfStepsAfterHold) #1*s and 150 steps
#
#time = rampQuadrupoleCurrent(startTime = time, endTime = time+1000*ms, startCurrent = varFullMagneticTrapCurrent, endCurrent = imagingCurrent, numberOfSteps = 150) #1*s and 150 steps

#closeBluePlugShutter(time)
time+=holdTimeAfterRampToImaging

### Wait for eddys to die down
#time += 50*ms


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
#    event(sfaRemoteCurrentSetVoltage, time, 0)
#    triggerTime = time-0.6*ms+2*ms                                    ###Commented SMD 08/04/11
    triggerTime = time-0.6*ms+2*ms-1.5*s                                ###ADDED SMD 08/04/11
    #time = rampDownQuadCoils(time-0.6*ms+2*ms, rapidOff = True)
    time = snapOffField(time-0.6*ms+2*ms)
    time += 700*us               #allow fields to shut off; leave time for repump pulse
    if(opticallyPlugTrap):
        closeBluePlugShutter(time)
#else:
#    triggerTime = time-0.6*ms+2*ms

if(not insituImage) :
    time += dtDriftTime

if(highFieldImage) :
    repumpTime = highFieldImageTime
else :
    repumpTime = time
    openMOTShutters(time)

### repump out of F = 1'
if (repumpMethod == opticallyPump) :
    repumpTimeDone = repumpMOT(repumpTime + 10*us, pumpingTime = 100*us)
    dtRepump = repumpTimeDone - repumpTime
elif (repumpMethod == microwavePump):
    repumpTimeDone = microwaveRepump(repumpTime + 10*us, pumpingTime = 100*us)
    dtRepump = repumpTimeDone - repumpTime
elif (repumpMethod == none):
    dtRepump = 110*us
   
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
    triggerTime = time-0.6*ms+2*ms
    #time = rampDownQuadCoils(time + (dtDeadMOT)/10, rapidOff = True)
    time = snapOffField(time + (dtDeadMOT)/10)
    


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

event(digitalSynch, triggerTime - 10*ms,0) #trigger webscope for quad coil ramp
event(digitalSynch, triggerTime,1) 
event(digitalSynch, triggerTime + 10*ms,0)

### Turn on MOT steady state ##################################################

MOT(time + 150*ms, leaveOn=True, cMOT = False)    # turn MOT back on


