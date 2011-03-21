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

setvar('imageCropVector',(530, 500, 250))
#setvar('imageCropVector',(581, 500, 100))
#setvar('imageCropVector',(500, 500, 490))

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 0.1*ms)

setvar('dtRamp1', 1.0*s)
setvar('dtRamp2', 1.00*s)
setvar('dtRamp3', 1.0*s)
setvar('dtRamp4', 1.0*s)

setvar('f0', 140)
setvar('f1', 70)

setvar('vca1', 6.75)
setvar('vca2', 5.0)
setvar('vca3', 4.20)
setvar('vca4', 4.20)

#setvar('holdTime', dtRamp1)
#setvar('holdTimeSequence', 1*s)
setvar('holdTime', 0.1*s)

setvar('MOTLoadTime', 2*s )
setvar('magneticTrap', True)
setvar('opticallyPump', False)

setvar('trapCurrentSequence', 40)

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

setvar('xAlign', 0)
setvar('zAlign', -5.5)

setvar('opticalPlug', False)
setvar('plugFocus',19.7)


#setvar('desc', "Mag Trap check - evaporate @ 300 - F=1, 210 MHz 30A CMOT - 0 MHz probe,  125-72.5 in 5*s @ 6.75 VCA, 72.5-36.25 in 1.0*s @ 5.0 VCA, MW dds @ 62.5 MHz resonance, 1% power, 20*ms reduced repump, 0V VCA, 0.01*ms cMOT, 22*ms sweep, with rapid off, with plug, with shutter, walking plug alignment (X,0) (Z, -4.2) x plus towards jason, z plus is against gravity, plugFocus 19.7, imaging on F=1")

#setvar('desc', "Mag Trap check - evaporate @ 180 - F=1, 210 MHz 30A CMOT - 0 MHz probe,  135-62.5 in 1*s @ 6.75 VCA, MW dds @ 62.5 MHz resonance, 1% power, 20*ms reduced repump, 0V VCA, 0.01*ms cMOT, 22*ms sweep, with rapid off, with plug, with shutter, walking plug alignment (X,0) (Z, -5.5) x plus towards jason, z plus is against gravity, plugFocus 19.7, imaging on F=1, probe light @ 25% for 25 us")

setvar('desc', "Mag Trap @ 40 A check - imaging on F=1, probe light @ 25% for 25*us, F=230 MHz, no MW, 0.1*s hold, NO plug with shutter, 2*s load ")

#setvar('desc', "cmot check, image on F=1, probe light @ 25% for 25 us")


# Global definitions


t0 = 2*ms

event(starkShiftingAOM, 100*us, (75, 0,0))
event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light
event(opticalPumpingBiasfield, t0 - 10*us, 0) # turn off optical pumping bias field
event(ddsRfKnife, t0 + 1*ms, (182.5, 0, 0))
event(rfKnifeAmplitude, t0 + 100*us, 0) # set to full attenuation on dds rf knife
event(sixPointEightGHzSwitch, t0, 0)



#### Make a mot ####
time = t0


setvar('varCMOTCurrent', 30)

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 22*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 8, dtCMOT = 0.01*ms, powerReduction = cmotPower, CMOTFrequency = 210, dtNoRepump = timeNoRepump, repumpAttenuatorVoltage = vcaVoltage, cmotCurrentRampRate = 1.0)


# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)


if (magneticTrap) :
    setvar('varFullMagneticTrapCurrent', trapCurrentSequence)
    setvar('varChargeCurrent', varCMOTCurrent)
    setvar('varDischargeCurrent', varFullMagneticTrapCurrent)
    time = evaporate(time, dtHold = holdTime, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, cmotCurrent = varCMOTCurrent, usePreCharge = False, chargeCurrent = varChargeCurrent, rapidOff = False, dischargeCurrent = varDischargeCurrent, makeRfCut = rfOn)
    # digital trigger
    event(ch(digitalOut, 4), time - 500*us, 1)
    event(ch(digitalOut, 4), time + 1*ms, 0)
    
else :
    tOff = time
    setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
    event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
    event(quadrupoleOnSwitch, tOff, 0)

#### Drift ###
time = time + dtDriftTime

##Image
if(realTime):
    dtDeadMOT = 100*ms
else:
    dtDeadMOT = 700*ms

if(realTime) : 
         ## Take an absorbtion image using Andor Solis Software ##
    if(fluorescenceImage) :
        time = takeSolisSoftwareFluorescenceImage(time, dtFluorescenceExposure = 2*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 5)
        
    else :
        time = takeSolisSoftwareAbsorptionImage (time, 0.1*ms)
    time = time + dtDeadMOT
    time = setQuadrupoleCurrent(time, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = varDischargeCurrent, rampRate = 1)

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

    time = setQuadrupoleCurrent(time + 11.345*ms, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = varDischargeCurrent, rampRate = 1)

      
    ## Turn on MOT steady state

#time = setQuadrupoleCurrent(time + 1*s, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 300, rampRate = 1)

if(realTime):
    tTAEndOfSequence = time +0.1*s
else:
    tTAEndOfSequence = time +1*s

time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False, motQuadCoilCurrent = 8)    # turn MOT back on

event(ch(digitalOut, 23), time + 10*s, 0)
event(ch(digitalOut, 23), time + 12.1*s, 1)
event(ch(digitalOut, 4), time + 12.2*s, 1)
event(ch(digitalOut, 4), time + 12.25*s, 0)








