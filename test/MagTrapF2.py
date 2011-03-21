from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

include('channels.py')
include('motFunction.py')
include('andorCameraFunctions.py')
include('evaporativeCoolingFunction.py')
include('repumpFunction.py')
include('depumpFunction.py')

setvar('imageCropVector',(581, 500, 100))
#setvar('imageCropVector',(500, 500, 490))

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 0.001*ms)

setvar('dtRamp1', 3.3*s)
setvar('dtRamp2', 2.25*s)
setvar('dtRamp3', 1.6*s)
setvar('dtRamp4', 1.0*s)

setvar('vca3', 4.20)
setvar('vca4', 4.20)

#setvar('holdTime', dtRamp1 + dtRamp2 + dtRamp3 + dtRamp4)
setvar('holdTime', 0.01*s)

setvar('MOTLoadTime', 2*s )
setvar('magneticTrap', True)
setvar('opticallyPump', True)

setvar('realTime', False)
setvar('fluorescenceImage', False)

setvar('zBiasL', 4.20) 
setvar('zBiasR', 5.48)
setvar('yBiasL', 0.0)
setvar('yBiasR', 4.351) 
setvar('xBiasL', -2.55) 
setvar('xBiasR', 2.12)
setvar('topBias1', 0.06)
setvar('topBias2', -0.05)
setvar('quadrupoleCompensation', -7.61)

setvar('vcaVoltage', 0)
setvar('timeNoRepump', 20*ms)
setvar('dtOpticalPumping', 100*us)
setvar('cmotPower', 0.01)


setvar('rfOn', False)
setvar('deltaFreq', 80e+006)
setvar('rbResonanceFreq', 6.834682610e+009 - 1e+003)
setvar('resFreq', rbResonanceFreq)
setvar('microwavePower', 10)
setvar('xAlign', 0)
setvar('zAlign', 0)

setvar('opticalPlug', True)


#setvar('desc', "Mag Trap check - evaporate @ 300 - F=1, 210 MHz 30A CMOT - 0 MHz probe,  135-72.5 in 3.3*s @ 6.75 VCA, 72.5-36.25 in 2.25*s @ 4.75 VCA, 36.25-18 in 1.6*s @ 4.20 VCA, 18-10 in 1.0*s @ 4.20 VCA, MW dds @ 62.5 MHz resonance, 1% power, 20*ms reduced repump, 0V VCA, 0.01*ms cMOT, 22*ms sweep, with rapid off, with plug, NO shutter, walking plug alignment (X,+0) (Z,+0) x plus towards jason, z plus is against gravity")

setvar('desc', "mag trap F=2")


# Global definitions


t0 = 200*ms

#if(rfOn):
###    event(ch(hp83711b, 3), t0-180*ms, microwavePower)
#    event(rfKnifeFrequency, t0 - 180*ms, resFreq - 40e6)

event(starkShiftingAOM, 100*us, (75, 0,0))
event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light
event(opticalPumpingBiasfield, t0 - 10*us, 0) # turn off optical pumping bias field
event(ddsRfKnife, t0 + 1*ms, (182.5, 0, 0))
event(rfKnifeAmplitude, t0 + 100*us, 0) # set to full attenuation on dds rf knife
event(sixPointEightGHzSwitch, t0, 0)



#### Make a mot ####
time = t0


setvar('varCMOTCurrent', 30)
#setvar('varCMOTCurrent', 0)

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 22*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 8, dtCMOT = 0.01*ms, powerReduction = cmotPower, CMOTFrequency = 210, dtNoRepump = timeNoRepump, repumpAttenuatorVoltage = vcaVoltage, cmotCurrentRampRate = 1.0)


if (opticallyPump) :
    time = repumpMOT(time + 1*us, pumpingTime = 1000*us)


if (magneticTrap) :
    setvar('varFullMagneticTrapCurrent', 300)
    setvar('varChargeCurrent', varCMOTCurrent)
    setvar('varDischargeCurrent', 300)
    time = evaporate(time, dtHold = holdTime, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, cmotCurrent = varCMOTCurrent, usePreCharge = False, chargeCurrent = varChargeCurrent, rapidOff = False, dischargeCurrent = varDischargeCurrent, makeRfCut = rfOn)
    
else :
    tOff = time
    setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
    event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
    event(quadrupoleOnSwitch, tOff, 0)



#### Drift ###
#time = time + dtDriftTime

##### repump out of F = 1' #####
#if (opticallyPump) :
#    time = repumpMOT(time + 10*us, pumpingTime = 1000*us)




##Image
dtDeadMOT = 700*ms

if(realTime) : 
         ## Take an absorbtion image using Andor Solis Software ##
    if(fluorescenceImage) :
        time = takeSolisSoftwareFluorescenceImage(time, dtFluorescenceExposure = 2*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 5)
        
    else :
        time = takeSolisSoftwareAbsorptionImage (time, 0.1*ms)
    time = time + dtDeadMOT
    time = setQuadrupoleCurrent(time, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 300, rampRate = 1)

else : 
    #    meas(absoptionLightFrequency, t0)
        ### Andor Camera ###
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    print time
    takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)
    # digital trigger
    event(ch(digitalOut, 4), time + dtDeadMOT - 500*us, 1)
    event(ch(digitalOut, 4), time + dtDeadMOT + 1*ms, 0)
    time = setQuadrupoleCurrent(time + 11.345*ms, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 300, rampRate = 1)
    

      
    ## Turn on MOT steady state

#time = setQuadrupoleCurrent(time + 1*s, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 300, rampRate = 1)

tTAEndOfSequence = time +1*s
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False, motQuadCoilCurrent = 8)    # turn MOT back on

#event(ch(digitalOut, 23), time + 2*s, 0)
#event(ch(digitalOut, 23), time + 2.1*s, 1)








