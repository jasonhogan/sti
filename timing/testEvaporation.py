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
include('evaporativeCoolingFunctionLinear2withDDS.py')
include('motShutters.py')

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDrift6.643E9imeSequence)
setvar('dtDriftTime', 0.1*ms) #0.1

#setvar('dtPlugOff', 50*ms)
#
#setvar('holdTimeSequence', 1*s)
#setvar('holdTimeAfterEvap', holdTimeSequence)
setvar('holdTimeBeforeEvap', 0*s) #0
setvar('holdTimeAfterEvap', 0*s) #0 #0.3
setvar('dtHighFieldImageOffset', 0*ms) #0
setvar('holdTimeAfterRampToImaging', 0*ms) #100
setvar('numberOfRamps', 5)



setvar('decompressAfterRamp', 2)

setvar('plugHorizontal', 12.5)
setvar('plugVertical', 18.0)

setvar('dtRampToImaging', 1000*ms)#125*ms

#setvar('heatingHoldCurrent', 135)
#setvar('dtHeatingHoldTimeMidRamp',5000*ms)

setvar('desc', "Evaporation; " + str(MOTLoadTime/s) + " s load; Hold time before evaporation " + str(holdTimeBeforeEvap/s) + " s; Hold time after evaporation " + str(holdTimeAfterEvap/s) + " s; Drift time " + str(dtDriftTime/ms) + " ms; Evaporation Ramps: " + str(numberOfRamps) + "; Image Detuning: " +  str(deltaImagingFreqMHz) +" MHz.")

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
setvar('applyFilterRF', False)

opticallyPump, microwavePump, none = range(3)
repumpMethod = opticallyPump

if(not realTime) :
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)

#setvar('repumpVCAseq',10)
#setvar('repumpVCA',repumpVCAseq)
#setvar('repumpVCA', 1)

#setvar('zLeft', 1.35)
#setvar('zRight', 1.0)

######## Prepare atom cloud ########

tStart = 100*us +1*s

event(imagingDetuning, tStart, 80 + deltaImagingFreqMHz)
###event(imagingOffsetFrequency, tStart, imagingResonanceFreq + deltaImagingFreqMHz*1e6)


#event(hpMicrowaveCarrier, tStart+10*us, microwaveCarrierFreq)

time = tStart + 5*ms

#meas(masterLockCheck, tStart, "Master lock check at start of sequence.")
#meas(imagingLockCheck, tStart, "Imaging lock check at start of sequence.")

#setvar('zShiftCM', 0.7)
#setvar('yShiftCM', -1.5) #-1.5
#setvar('xShiftCM', -1)
#setvar('zShiftCM', 0.68)
#setvar('yShiftCM', -1.78)
#setvar('xShiftCM', 0.39)
setvar('zShiftCM', -0.) #-0.2
setvar('yShiftCM', -0.) #-0.4
setvar('xShiftCM', 0.) #0.2

event(zBiasTop, time, zLeft + zShiftCM)
event(zBiasBot, time+10*us, zRight - zShiftCM)
event(xBiasLeft, time+20*us, xLeft + xShiftCM)
event(xBiasRight, time+30*us, xRight + xShiftCM)
event(yBiasLeft, time+40*us, yLeft + yShiftCM)
event(yBiasRight, time+50*us, yRight - yShiftCM)

##########################
######## Safety ##########
##########################

event(topSafetySwitch, time + 100*us, 0)
event(topVCA, time + 100*us, 0)

time += 1*ms

#if(imagingCurrentBEC != 58):
#    print 1/0

time = makeCMOT(time)

#takeFluorescenceExposure(time - 2*s, dtFluorescenceExposure=1*ms, cropVector = imageCropVector)
time = turn2DMOTLightOff(time)
time = turnMOTLightOff(time)

#microwavePulse(time + 0.5*ms-10*us, pulseTime = 1000*us)

time = depumpMOT(time + 200*us + 410*us, pumpingTime = 70*us, ta4PowerFrac = 1.0)  #200*us #70*us

triggerTime = time

#time += 50*ms #time for atoms to move from cMOT center to mag trap center at 35 G/cm


time = snapOnField(time+0.5*ms, snapCurrent = magTrapTransferCurrent)
#setQuadrupoleCurrent(time+0.5*ms, current = magTrapTransferCurrent)
sfaCurrent = magTrapTransferCurrent



time += 300*ms


#event(hpMicrowaveCarrier, time+0.5*ms, microwaveCarrierFreq)

time = rampQuadrupoleCurrent(startTime = time, endTime = time+250*ms, startCurrent = magTrapTransferCurrent, endCurrent = varFullMagneticTrapCurrent, numberOfSteps = 150)
#time = rampUpQuadCoils(time, True, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, chargeCurrent = CMOTcurrent, quadRampRate = 0.5)

time += holdTimeBeforeEvap

if(opticallyPlugTrap):
    openBluePlugShutter(time)
else:
    closeBluePlugShutter(time)

if (numberOfRamps > 0) :
    closeMOTShutters(time)

intermediateRampRatio = (1.0*(varFullMagneticTrapCurrent - intermediateTrapCurrent) / (varFullMagneticTrapCurrent - imagingCurrentBEC))

fullDarkZ(time + 5*ms)
time = evaporate(time, rampNumber = numberOfRamps)
endFullDarkZ(time - 5*ms)

#time += 10*us
#event(rfKnifeAmplitude, time, 10)
#event(sixPointEightGHzSwitch, time, 1)
#event(ddsRfKnife, time, (ddsRbResonanceFreq,100,0))



#setvar('dtIntermediateRamp', dtRampToImaging*intermediateRampRatio)
#setvar('tRampStart', time + 10*us)

#time += dtIntermediateRamp

#closeBluePlugShutter(time)
time += holdTimeAfterEvap

#event(rfKnifeAmplitude, time, 0)
#event(sixPointEightGHzSwitch, time, 0)

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

if (decompressAfterRamp < numberOfRamps) :
    rampQuadrupoleCurrent(startTime = tRampStart, endTime = tRampStart + dtIntermediateRamp, startCurrent = varFullMagneticTrapCurrent, endCurrent = intermediateTrapCurrent, numberOfSteps = 150*intermediateRampRatio)
    time = rampQuadrupoleCurrent(startTime = time, endTime = time+dtRampToImaging*(1 - intermediateRampRatio), startCurrent = intermediateTrapCurrent, endCurrent = imagingCurrentBEC, numberOfSteps = 150*(1 - intermediateRampRatio), quadCoilIsCC = True) #1*s and 150 steps
else:
    time = rampQuadrupoleCurrent(startTime = time, endTime = time+dtRampToImaging, startCurrent = varFullMagneticTrapCurrent, endCurrent = imagingCurrentBEC, numberOfSteps = 150) #1*s and 150 steps
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

#setvar('zShiftCM', 0)
#setvar('zShiftDiff', -0.56) #-0.05
#setvar('zLeft', 2.22 + zShiftCM + zShiftDiff)    # 2.22
#setvar('zRight', 1.58 + zShiftCM -  zShiftDiff)  # 1.58
#setvar('timeZBiasRamp', time - 0*s)
#
#event(zBiasTop, timeZBiasRamp, zLeft-0)
#event(zBiasBot, timeZBiasRamp+10*us, zRight+0)

#closeBluePlugShutter(time)
#setvar('plugOffTime', time+10*ms)
#time+=holdTimeAfterRampToImaging

### Wait for eddys to die down
#time += 50*ms


#if(opticallyPlugTrap):
#    closeBluePlugShutter(time)

#openBluePlugShutter(time)
#time += 2*ms


##close shutter at high field
#closeBluePlugShutter(highFieldImageTime - dtPlugOff)

######## Measure the result: Snap off field, repump, and image ########

#triggerTime = time

if(not insituImage) :
    #### Snap off magnetic field
#    time = rampDownQuadCoils(time, rapidOff = True)
#    time = rampDownQuadCoils(time, fullMagneticTrapCurrent = varDischargeCurrent, dischargeCurrent = 0, rapidOff = False,  quadRampRate = 2)
#    event(sfaRemoteCurrentSetVoltage, time, 0)
#    triggerTime = time-0.6*ms+2*ms                                    ###Commented SMD 08/04/11
#    triggerTime = time-0.6*ms+2*ms-1.5*s                                ###ADDED SMD 08/04/11
    #time = rampDownQuadCoils(time-0.6*ms+2*ms, rapidOff = True)
    tSnapOffField = time-0.6*ms+2*ms
#    triggerTime = tSnapOffField-1*ms
    time = snapOffField(tSnapOffField)
#    setQuadrupoleCurrent(tSnapOffField, current = 0)
    time += 700*us               #allow fields to shut off; leave time for repump pulse

#    triggerTime = tSnapOffField
    
    if(opticallyPlugTrap):
        setvar('plugOffTime', time+10*ms)
        closeBluePlugShutter(plugOffTime) #+20*ms 
#else:
#    triggerTime = time-0.6*ms+2*ms

if(not insituImage) :
    time += dtDriftTime

if(highFieldImage) :
    repumpTime = highFieldImageTime
else :
    time += 0*ms
    repumpTime = time
    openMOTShutters(time)

### repump out of F = 1'

setvar('repumpDuration', 200*us)



if (repumpMethod == opticallyPump) :
    repumpTimeDone = repumpMOT(repumpTime + 10*us, pumpingTime = repumpDuration, ta4PowerFrac=1.5)
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
    #triggerTime = time-0.6*ms+2*ms
    #time = rampDownQuadCoils(time + (dtDeadMOT)/10, rapidOff = True)
    time = snapOffField(time + (dtDeadMOT)/10)
    

#triggerTime = imageTime


if(realTime) : 
    # Take an absorbtion image using Andor Solis Software
#    imageDoneTime = takeSolisSoftwareAbsorptionImage(imageTime, expTime = 50*us, dtAbsorbtionLight = 35*us, iDus = False, depumpAbsImage = depumpImage)
#    imageDoneTime = takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, iXonImage = True, imagingDetuning = 0)
    imageDoneTime = takeSolisSoftwareFluorescenceImage(imageTime+10*us, dtFluorescenceExposure = 2*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)
 
else :
#    triggerTime = imageTime
    imageDoneTime = takeAbsorptionImage(imageTime, time + dtDeadMOT, cropVector=imageCropVector, depumpAbsImage = depumpImage)
#    imageDoneTime = takeFluorescenceImage(imageTime, dtFluorescenceExposure=10*ms, leaveMOTLightOn=False, cropVector = imageCropVector, repumpOn = True)

#                               takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)
#    imageDoneTime = takeSolisSoftwareFluorescenceImage(imageTime+300*us, dtFluorescenceExposure = 2*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)
#    takeSolisSoftwareImage(imageTime+200*us, dtExposure = 1*ms, iDusImage = True)
#    repumpDepump(imageTime+200*us, pumpingTime = 2*ms)

time += (imageDoneTime - imageTime) 
###############################################################


if(opticallyPlugTrap and insituImage):
    closeBluePlugShutter(time+100*ms)


#closeBluePlugShutter(time+100*ms)

event(digitalSynch, triggerTime - 10*ms,0) #trigger webscope for quad coil ramp
event(digitalSynch, triggerTime,1) 
event(digitalSynch, triggerTime + 10*ms,0)

#event(zBiasTop, time+50*ms, zLeft)
#event(zBiasBot, time+50*ms+10*us, zRight)

### Turn on MOT steady state ##################################################
#
time+=200*ms
time = turn2DMOTLightOn(time + 150*ms)
tSteadStateMot = MOT(time + 150*ms, leaveOn=True, cMOT = False)    # turn MOT back on

#meas(masterLockCheck, tSteadStateMot, "Master lock check at end of sequence.")
#meas(imagingLockCheck, tSteadStateMot, "Imaging lock check at end of sequence.")

###Begin format
#
# <Name: MOT, Color: Red, Line {1,5}>
#
#
