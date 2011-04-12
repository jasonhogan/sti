###############################################################################
# Capabilities of file
#     0) Definitions and setvars             (channels.py)
#     1) Make a MOT                          (motFunction.py)
#     2) Depump into F = 1'                 (depumpFunction.py)
#     3) Evaporate                               (evaporativeCoolingFunction.py)
#     4) Drift
#     5) Repump into F = 1'                  (repumpFunction.py)
#     6) Image                                     (andorCameraFunctions.py)
#     7) Turn on MOT steady state        (motFunction.py)
###############################################################################


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

#setvar('imageCropVector',(608, 435 , 150))
setvar('imageCropVector',(500, 500, 490))

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 0.1*ms)


### VCAs ######################################################################
## VCA's control RF knife ramps
## ~4.5 is off. Frequencies are in MHz.
## Power should drop and ramps should get faster as you evaporate. 
## Several ramps are defined here, but not all may be executed. See evaporativeCoolingFunction
setvar('vca1', 6.5)
setvar('f0', 120)
setvar('f1', 60)
setvar('dtRamp1', 4.75*s)

setvar('vca2', 5.75)
setvar('f2', 30)
#setvar('dtRamp2Sequence', 1*s)
setvar('dtRamp2', 1.5*s)  #2.0*s

setvar('vca3', 5.75)
setvar('f3', 15)
setvar('dtRamp3', 0.6*s)

setvar('vca4', 5.5)
setvar('f4', 9)
setvar('dtRamp4Sequence', 1*s)
setvar('dtRamp4', 0.15*s) #1.1*s

setvar('vca5', 5.0)
setvar('f5', 6)
setvar('dtRamp5Sequence', 1*s)
setvar('dtRamp5', 0.15*s)

setvar('vca6', 5.0)
setvar('f6', 3)
setvar('dtRamp6', 0.15*s)
###############################################################################


#setvar('holdTimeSequence', 1*s)
#setvar('holdTime', holdTimeSequence)
setvar('holdTime',dtRamp1 + dtRamp2 + dtRamp3 + dtRamp4 + dtRamp5) #holdTime must include all executed dtRamps
#setvar('holdTime',dtRamp1) #holdTime must include all executed dtRamps
#setvar('holdTime', 0.1*s)

# + dtRamp2   + dtRamp3 + dtRamp4 + dtRamp5
setvar('MOTLoadTime', 5*s )

## The following must all be true for full evaporation
setvar('magneticTrap', True)
setvar('opticallyPump', True)
setvar('usePreCharge', True)
setvar('evaporateAtoms', False)
setvar('opticallyPlugTrap', True)

##Logging of trim coils
setvar('zBiasL', 1.35) 
setvar('zBiasR', 4.5)
setvar('yBiasL', 1.4)
setvar('yBiasR', 3.4) 
setvar('xBiasL', -4.8) 
setvar('xBiasR', 1.3)
setvar('topBias1', 0.2)
setvar('topBias2', 0)

setvar('rfOn', False)


### Descriptions ##############################################################
#setvar('desc', "5*s MOT --> 180 MHz CMOT, 8A, ramp @ RR=2.0 to 35A,  image on F=2 @ d = 0, ramp to 300A and back down, 5*s hold, evaporate:  (6*s @ 6.25, 120->60), (2.0*s @ 5.75, 60->30), (1.25*s @ 5.75, 30->15), (1.1*s @ 5.5, 15->9), (1.00*s @ 5.0, 9->7.8)")

#setvar('desc', "5*s MOT --> 180 MHz CMOT, 8A, ramp @ RR=2.0 to 35A,  image on F=2 @ d = 0, ramp to 300A and back down, 5*s hold, evaporate:  (6*s @ 6.25, 120->60)")
#
#setvar('desc', "5*s MOT --> 180 MHz CMOT, 8A, ramp @ RR=2.0 to 35A,  image on F=2 @ d = 0, 3 step evap, 3.1*s hold")

#setvar('desc', "10*s MOT --> 180 MHz CMOT, 8A, ramp @ RR=2.0 to 35A,  image on F=2 @ d = 5, 1 step evap, 6W plug only @ high field, plug aligned at high field")

#setvar('desc', "5*s MOT --> 180 MHz CMOT, 8A, ramp @ RR=2.0 to 35A,  image on F=2 @ d = 0, 5 step evap, z=-1, y=0, 6W plug only @ high field, plug aligned at high field")

setvar('desc', "5*s MOT --> 180 MHz CMOT, 8A, ramp @ RR=2.0 to 35A,  image on F=2 @ d = 0, ramp to 300A and back down, 0.1*s hold Plug")

## for zPos and yPos: right-hand rule = +
#look at lifetime after evaporation
###############################################################################


setvar('probeFrequency', 174)

setvar('plugPower', 6) ## Watts
setvar('zPos',2)
setvar('yPos',3)
setvar('greenFocus', 21.50)

# Global definitions


t0 = 2*ms

event(ch(digitalOut, 22), t0, 1)
event(ch(digitalOut, 22), t0 + 1*s, 0)

setvar('deltaImagingFreqSequence', 10e+06)
setvar('deltaImagingFreq', 0e+06)
event(imagingOffsetFrequency, t0, 1.802e+09 + deltaImagingFreq)

event(probeLightRFSwitch, t0, probeLightOff)             # AOM is off, so no imaging ligh
event(probeLightShutter, t0+1*ms, 0)

event(ddsRfKnife, t0 + 1*ms, (180, 0, 0))
event(sixPointEightGHzSwitch, t0, 0)

#event(capacitorPreChargeRelay, t0 - 1*ms, 5)
#event(capacitorPreChargeRelay, t0 + 2*s, 0)
#event(preChargeVoltage, t0-10*us, 1) # set cap voltage to 45V using Lambda

event(quadCoilShuntSwitch, t0-134*us, 1)

# digital trigger
event(ch(digitalOut, 4), t0 + 2*s - 500*us, 1)
event(ch(digitalOut, 4), t0 + 2*s + 1*ms, 0)


### Make a mot ################################################################
time = t0

setvar('varCMOTCurrent', 8)
setvar('chargeTime', 15*ms)

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 20*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 8, dtCMOT = 1*ms, powerReduction = 1.0, CMOTFrequency = 180, dtNoRepump = 5*ms, repumpAttenuatorVoltage = 0, cmotCurrentRampRate = 1)

# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)
###############################################################################


### Depump into F = 1' ########################################################
time = depumpMOT(time + 10*us, pumpingTime = 100*us)
###############################################################################


### Evaporate #################################################################

if (usePreCharge) :
    setvar('preChargeCurrent', 35)
    preChargeTime = time - 10*ms
    setQuadrupoleCurrent(preChargeTime, preChargeCurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 8, rampRate = 2.0)
    time = preChargeTime + chargeTime

if (magneticTrap) :
    setvar('varFullMagneticTrapCurrent', 300)
    setvar('varChargeCurrent', preChargeCurrent)
    setvar('varDischargeCurrent', 35) 
    ### If varDischargeCurrent is > 35, rapidOff below MUST BE FALSE or you will break things

#    time = time + 100*ms
#    event(quadCoilShuntSwitch, time, 0) ## shunt the coil to a new location, out of the 2D beam
#    time = time + (holdTime - 200*ms)
#    event(quadCoilShuntSwitch, time, 1) ## shunt the coil to a new location, out of the 2D beam
    

    ### If varDischargeCurrent is > 35, rapidOff below MUST BE FALSE or you will break things
    time = evaporate(time, dtHold = holdTime, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, cmotCurrent = varCMOTCurrent, usePreCharge = False, chargeCurrent = varChargeCurrent, rapidOff = True, dischargeCurrent = varDischargeCurrent, makeRfCut = evaporateAtoms, usePlug = opticallyPlugTrap)


### Ask Dr. Johnson
#    time = time + 100*ms

#    tOff = time
#    setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
#    event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
#    event(quadrupoleOnSwitch, tOff, 0)

else :
    tOff = time
    setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
    event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
    event(quadrupoleOnSwitch, tOff, 0)
###############################################################################


### Drift #######################################################################
time = time + dtDriftTime
###############################################################################


### repump out of F = 1' ######################################################
if (opticallyPump) :
    time = repumpMOT(time + 10*us, pumpingTime = 100*us)
###############################################################################


### Image #####################################################################
dtDeadMOT = 500*ms

#tOff = time + dtDeadMOT/2
#setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
#event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
#event(quadrupoleOnSwitch, tOff, 0)


setvar('realTime', True)

if(realTime) : 
         ## Take an absorbtion image using Andor Solis Software ##
    time = takeSolisSoftwareAbsorptionImage (time, expTime = 75*us, dtAbsorbtionLight = 25*us, iDus = True)

#    time = setQuadrupoleCurrent(time + 11.345*ms, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = varDischargeCurrent, rampRate = 1)

else : 
    #    meas(absoptionLightFrequency, t0)
        ### Andor Camera ###
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    print time
    imageTime = time
    time = takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)

#    closeBluePlugShutter(imageTime + 100*ms)

    takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)

#    time = setQuadrupoleCurrent(time + 11.345*ms, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = False, startingCurrent = varDischargeCurrent, rampRate = 1)
###############################################################################
    

      
### Turn on MOT steady state ##################################################
tTAEndOfSequence = time + 1500*ms
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False)    # turn MOT back on
############################################################################

#
event(ch(digitalOut, 4), time + 1.5*s - 1*ms, 1)
event(ch(digitalOut, 4), time + 1.5*s, 0)
#






