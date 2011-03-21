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

#setvar('imageCropVector',(570, 500, 300))
#setvar('imageCropVector',(500, 500, 300))
setvar('imageCropVector',(500, 500, 490))

setvar('dtDriftTimeSequence', 1000*us)
setvar('dtDriftTime', dtDriftTimeSequence)
#setvar('dtDriftTime', 0.1*ms)

setvar('dtRamp1', 1.0*s)
setvar('dtRamp2', 1.0*s)
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
setvar('holdTime', 0.10*s)

setvar('MOTLoadTime', 1*s )
setvar('magneticTrap', False)
setvar('imageAtHighField', False)
setvar('opticallyPump', True)
setvar('ramanTransition', False)

#setvar('trapCurrentSequence', 40)

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
setvar('cmotPower', 0.71)

setvar('dtRamanPulseSequence', 2.2*us)

setvar('rbResonanceFreq', 6.834682610e+009 - 1e+003)
setvar('resFreq', rbResonanceFreq)
setvar('deltaFreq', 0e+003)

setvar('rfOn', False)

setvar('xAlign', 0)
setvar('zAlign', -5.5)

setvar('opticalPlug', False)
setvar('plugFocus',19.7)


setvar('desc', "CMOT, 100*us depump pulse, imaging on F=2, probe light @ 25% for 25*us, F=185 MHz, with shutter on TA2 input light, scan raman pulse, with TA5, with rf, with atom filter")



# Global definitions


t0 = 200*ms

event(TA4, t0 + 10*us, 0)
event(TA5, t0 + 10*us, 0.4) # leave ta5 on with no seed for fast current ramp
event(TA6, t0 + 10*us, 0)

event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light
event(ramanRfSwitch, t0 - 1*ms, 0)                  # no rf modulation on raman EOSpace fiber phase modulator
event(ramanShutter, t0, 1)                                  # open Raman Mechanical Shutter
event(motLightShutter, t0 + 100*us, 1)                                  # open ta2 mot light Mechanical Shutter

if(rfOn):
    event(ddsRfKnife, t0 + 1*ms, (182.5, 0, 0))

event(rfKnifeAmplitude, t0 + 100*us, 0) # set to full attenuation on dds rf knife

event(sixPointEightGHzSwitch, t0, 0)

event(rfKnifeFrequency, t0 - 180*ms, resFreq + 200e+06 + 600e+03)



#### Make a mot ####
time = t0


setvar('varCMOTCurrent', 0)

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 5*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 4, dtCMOT = 21*ms, powerReduction = cmotPower, CMOTFrequency = 210, dtNoRepump = timeNoRepump, repumpAttenuatorVoltage = vcaVoltage, cmotCurrentRampRate = 1.0)

event(repumpVariableAttenuator, time - 100*us, 0)
time = depumpMOT(time + 10*us, pumpingTime = 1000*us)

if (magneticTrap) :
    setvar('varFullMagneticTrapCurrent', 40)
    setvar('varChargeCurrent', varCMOTCurrent)
    setvar('varDischargeCurrent', 40)
    time = evaporate(time, dtHold = holdTime, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, cmotCurrent = varCMOTCurrent, usePreCharge = False, chargeCurrent = varChargeCurrent, rapidOff = (not opticalLatticeHold), dischargeCurrent = varDischargeCurrent, makeRfCut = rfOn)
    # digital trigger
    event(ch(digitalOut, 4), time - 500*us, 1)
    event(ch(digitalOut, 4), time + 1*ms, 0)
    
else :
    tOff = time
    setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
    event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
    event(quadrupoleOnSwitch, tOff, 0)
    event(ch(digitalOut, 4), time - 500*us, 1)
    event(ch(digitalOut, 4), time + 1*ms, 0)




### Apply a raman transition ###

# always need TA4 and either 5 or 6 to make a perpendicular pair
# TA5 is currently biased for full output
if(ramanTransition):
    #turnMOTLightOff(time + 10*us)
    event(TA4, time + 10*us, 0.8)
    #event(TA4, time + 2.45*ms, 0)
    event(TA5, time + 1.25*ms, 0.8)
    event(TA6, time + 10*us, 0)
    event(motLightShutter, time, 0)
    event(motFrequencySwitch, time - 200*us, 1) # turn off all cooling modulation
    time = time + 2.5*ms

    setvar('dtRamanPulse', dtRamanPulseSequence)
#    openBluePlugShutter(time - 1*ms)
#    voltageSweep(TA4, time, sweepTime = 10*ms, startVoltage = 0, stopVoltage = 0.8, numberOfEvents = 10)
#    voltageSweep(TA5, time + 1*us, sweepTime = 10*ms, startVoltage = 0, stopVoltage = 0.8, numberOfEvents = 10)

    ## switch off all mw power to cooling modulator & switch off all repump ##
#    event(repumpFrequencySwitchX,  time - 100*us, 1)                 # turn off repump
    



    ## turn on TA2
    event(TA2, time - 500*us, voltageTA2)
    event(TA7, time - 580*us, ta7MotVoltage)

    ## turn on TAs 4&5
    event(TA4, time, 0.8)
    event(TA5, time, 0.8)
    
    event(braggAOM2, time, (100, 100, 0))
    event(ramanRfSwitch, time, 1)
    event(ramanShutter, time - 4*ms, 1)

    time = time + dtRamanPulse
    event(TA4, time, 0.0)
    event(TA5, time, 0.0)

    event(ramanRfSwitch, time + 1*ms, 0)
#    event(ramanShutter, time, 0)
    event(braggAOM2, time + 1*ms, (100, 0, 0))
    closeBluePlugShutter(time + 4*ms)
    ## switch modulation back on ##
#    event(repumpFrequencySwitchX,  time + 100*us, 0)                 # turn on repump
    event(motFrequencySwitch, time + 200*us, 0)                         # turn on all cooling modulation
    event(TA2, time + 200*us, 0)                                                 # turn off TA2
    event(TA7, time + 400*us, 0)                                                 # turn off TA2


#### Drift ###
time = time + dtDriftTime

#### repump out of F = 1' #####
if (opticallyPump) :
    time = repumpMOT(time + 10*us, pumpingTime = 1000*us)

##Image
if(realTime):
    dtDeadMOT = 100*ms
else:
    dtDeadMOT = 100*ms

if(realTime) : 
         ## Take an absorbtion image using Andor Solis Software ##
    if(fluorescenceImage) :
        time = takeSolisSoftwareFluorescenceImage(time, dtFluorescenceExposure = 2*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 5)
        
    else :
        time = takeSolisSoftwareAbsorptionImage (time, 0.1*ms)
    time = time + dtDeadMOT
    if(imageAtHighField):
        time = setQuadrupoleCurrent(time, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = varDischargeCurrent, rampRate = 1)

else : 
    #    meas(absoptionLightFrequency, t0)
        ### Andor Camera ###
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    print time
    takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)

#    # digital trigger
#    event(ch(digitalOut, 4), time + dtDeadMOT - 500*us, 1)
#    event(ch(digitalOut, 4), time + dtDeadMOT + 1*ms, 0)

    if(imageAtHighField):
        time = setQuadrupoleCurrent(time + 11.345*ms, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = varDischargeCurrent, rampRate = 1)

      
    ## Turn on MOT steady state

#time = setQuadrupoleCurrent(time + 1*s, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 300, rampRate = 1)

if(realTime):
    tTAEndOfSequence = time +0.1*s
else:
    tTAEndOfSequence = time +1*s

time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False, motQuadCoilCurrent = 8)    # turn MOT back on
event(ramanShutter, time, 1)
event(motLightShutter, time + 100*us, 1)                                  # open ta2 mot light Mechanical Shutter

#
event(TA5, time + 10*us, 0)
event(TA6, time + 10*us, 0)


#event(ch(digitalOut, 4), time + 12.2*s, 1)
#event(ch(digitalOut, 4), time + 12.25*s, 0)








