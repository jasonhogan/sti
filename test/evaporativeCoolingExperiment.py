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
setvar('dtDriftTime', 0.1*ms)

#setvar('holdTimeSequence', 1*s)
#setvar('holdTime', holdTimeSequence)
setvar('holdTime', 0.16*s)

setvar('rabiPulseTime', 600*us)

setvar('rbResonanceFreq', 6.834682610e+009 - 1e+003)
setvar('resFreq', rbResonanceFreq)

setvar('MOTLoadTime', 5*s )
setvar('magneticTrap', True)
setvar('opticallyPump', True)

#setvar('zBiasL', 4.27) 
#setvar('zBiasR', 5.37)
#setvar('yBiasL', 0.0)
#setvar('yBiasR', 4.35) 
#setvar('xBiasL', -2.50) 
#setvar('xBiasR', 2.40)
#setvar('topBias1', 0.09)
#setvar('topBias2', -0.05)

setvar('rfOn', False)
setvar('deltaFreq', 0e+003)



#setvar('desc', "molasses - with pull back to 180 MHz for 20*ms, with repump, 100*us depump pulse - pull power back to 100% TA command - 0.3*ms rabi pulse, scan from 20 to 220kHz in steps of 2kHz")

#setvar('desc', "cMOT check - 5*s load - 10 MHz detuned - 8A CMOT, 210 MHz for 20*ms @ 80% power, 0*ms with no repump, 1*ms sweep, 0*ms optical pumping, 145 MHz MOT freq")

#setvar('desc', "microwave spectroscopy - 1*s load, CMOT with 3*ms no repump - 0 MHz detuned probe - 200*us scanning from -20 kHz to +20 kHz detuned MW - 1000*us optical pumping")

#setvar('desc', "MOT check - 10*s load - 10 MHz detuned - 8A MOT, stock mag field alignment")

setvar('desc', "Mag Trap check - 0.16*s hold @ 45 - F=1, 200 MHz 8A CMOT - on resonance probe")

#setvar('desc',"MW Evaporation using new microwave horn position, and dds knife. 20*ms 50 MHz CMOT; rfCut: True; QuadCoil: 300 A; frequencyRamp: True; EvaporationTime: 3*s, 100*ms equillibration; Exponential Ramp 180 to 120 MHz in 3*s with tc = 1.25; 0.1*ms drift for TOF; Plug off")

setvar('probeFrequency', 174)

# Global definitions


t0 = 2*ms

if(rfOn):
    event(rfKnifeFrequency, t0, resFreq + deltaFreq)


#event(starkShiftingAOM, 100*us, (50,0,0))
#event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light
#event(opticalPumpingBiasfield, t0 - 10*us, 0) # turn off optical pumping bias field
#event(ddsRfKnife, t0 + 1*ms, (180, 0, 0))



#### Make a mot ####
time = t0

setvar('varCMOTCurrent', 35)

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 30*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 8, dtCMOT = 1*ms, powerReduction = 1.0, CMOTFrequency = 200, dtNoRepump = 0*ms, repumpAttenuatorVoltage = 0)

#210

if (opticallyPump) :
    time = depumpMOT(time + 1*us, pumpingTime = 1000*us)
else :
    time = time + 10*us


# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)


#

if (magneticTrap) :
    setvar('varFullMagneticTrapCurrent', 35)
    setvar('varChargeCurrent', 35)
    setvar('varDischargeCurrent', 35)
    time = evaporate(time, dtHold = holdTime, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, cmotCurrent = varCMOTCurrent, usePreCharge = False, chargeCurrent = varChargeCurrent, rapidOff = True, dischargeCurrent = varDischargeCurrent, makeRfCut = False)
    
else :
    tOff = time
    setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
    event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
    event(quadrupoleOnSwitch, tOff, 0)



#### Drift ###
time = time + dtDriftTime

#### repump out of F = 1' #####
if (opticallyPump) :
    time = repumpMOT(time + 10*us, pumpingTime = 1000*us)

### RF



if (rfOn) :
    dtCut = 0.3*ms
##    dtCut = rabiPulseTime
    event(sixPointEightGHzSwitch, time, 1)
    event(sixPointEightGHzSwitch, time + dtCut, 0)
    time = time + dtCut


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

#event(ch(digitalOut, 4), time + 4*s, 0)








