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

#setvar('imageCropVector',(514, 393 ,250))
setvar('imageCropVector',(500, 500, 490))

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 1*ms)

#setvar('holdTimeSequence', 1*s)
#setvar('holdTime', holdTimeSequence)
setvar('holdTime', 0.1*s)

setvar('vca1', 6)
setvar('f0', 100)
setvar('f1', 50)
setvar('dtRamp1', 10*s)


setvar('MOTLoadTime', 5*s )
setvar('magneticTrap', False)
setvar('opticallyPump', False)

setvar('zBiasL', 1.35) 
setvar('zBiasR', 4.5)
setvar('yBiasL', 1.4)
setvar('yBiasR', 3.4) 
setvar('xBiasL', -4.8) 
setvar('xBiasR', 1.3)
setvar('topBias1', 0.2)
setvar('topBias2', 0)

setvar('rfOn', False)

#setvar('desc', "Mag Trap check - 0.1*s hold @ 35 - F=1, 180 MHz 35A CMOT - on resonance probe")
setvar('desc', "20*s MOT --> CMOT, 35A  image on F=2, d=10 MHz")
#setvar('desc', "20*s MOT --> CMOT, 180 MHz 35A w/ depump & repump --> 50A mag trap for 15*s")

#setvar('desc',"MW Evaporation using new microwave horn position, and dds knife. 20*ms 50 MHz CMOT; rfCut: True; QuadCoil: 300 A; frequencyRamp: True; EvaporationTime: 3*s, 100*ms equillibration; Exponential Ramp 180 to 120 MHz in 3*s with tc = 1.25; 0.1*ms drift for TOF; Plug off")

setvar('probeFrequency', 174)

# Global definitions


t0 = 2*ms

setvar('deltaImagingFreqSequence', 10e+06)
setvar('deltaImagingFreq', deltaImagingFreqSequence)

event(imagingOffsetFrequency, t0, 1.8e+09 + deltaImagingFreq)

event(probeLightRFSwitch, t0, probeLightOff)             # AOM is off, so no imaging light

#event(opticalPumpingBiasfield, t0 - 10*us, 0) # turn off optical pumping bias field
event(ddsRfKnife, t0 + 1*ms, (180, 0, 0))
event(sixPointEightGHzSwitch, t0, 0)
event(zAxisCompCoil, t0 - 1*ms, 0)



#### Make a mot ####
time = t0

setvar('varCMOTCurrent', 35)

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 31*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 6, dtCMOT = 10*ms, powerReduction = 1.0, CMOTFrequency = 180, dtNoRepump = 0*ms, repumpAttenuatorVoltage = 0)



if (opticallyPump) :
    time = depumpMOT(time + 10*us, pumpingTime = 2000*us)
else :
    time = time + 10*us
#
#event(ch(dds, 1), time - 5*us, (134, 100, 0))
#event(TA2, time + 1.5*s, voltageTA2)                   # TA on
#event(TA3, time + 1.5*s + 10*us, voltageTA3)                   # TA on
#event(ta3SeedShutter, time + 1*s , 1) 



# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)


#

if (magneticTrap) :
    setvar('varFullMagneticTrapCurrent', 300)
    setvar('varChargeCurrent', 35)
    setvar('varDischargeCurrent', 35)

#    event(zAxisCompCoil, time + 100*ms, 5)

    time = evaporate(time, dtHold = holdTime, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, cmotCurrent = varCMOTCurrent, usePreCharge = False, chargeCurrent = varChargeCurrent, rapidOff = True, dischargeCurrent = varDischargeCurrent, makeRfCut = False)
#    event(zAxisCompCoil, time - 500*ms, 0)


else :
    tOff = time
    setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
    event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
    event(quadrupoleOnSwitch, tOff, 0)


#event(TA2, time - 1*s, 0)                   # TA on
#event(TA3, time - 1*s + 10*us, 0)                   # TA on

#### Drift ###
time = time + dtDriftTime

#### repump out of F = 1' #####
if (opticallyPump) :
    time = repumpMOT(time + 10*us, pumpingTime = 1000*us)

##Image
dtDeadMOT = 100*ms

setvar('realTime', False)

if(realTime) : 
         ## Take an absorbtion image using Andor Solis Software ##
    time = takeSolisSoftwareAbsorptionImage (time, 0.1*ms)
    
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

#event(zAxisCompCoil, time + 1*ms, 0)

#event(ch(digitalOut, 4), time + 4*s, 0)

event(zAxisCompCoil, time + 1*ms, 0)








