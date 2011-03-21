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
setvar('holdTime', 400*ms)

setvar('MOTLoadTime', 5*s )
setvar('magneticTrap', True)
setvar('opticallyPump', True)

setvar('realTime', False)
setvar('fluorescenceImage', False)

setvar('zBiasL', 4.27) 
setvar('zBiasR', 5.37)
setvar('yBiasL', 0.0)
setvar('yBiasR', 4.35) 
setvar('xBiasL', -2.50) 
setvar('xBiasR', 2.40)
setvar('topBias1', 0.09)
setvar('topBias2', -0.05)

setvar('desc', "Mag Trap check - 0.4*s hold @ 40 - F=1, 210 MHz 40A CMOT - 0 MHz probe, 10*ms no repump")
#setvar('desc', "cMOT check - 4.25A MOT, 40A cMOT, 11*ms at 210 MHz, 100% power, 40*ms sweep, 10*Mhz detuned imaging, 10*ms no repump")

# Global definitions


t0 = 2*ms

event(starkShiftingAOM, 100*us, (50,0,0))
event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light
event(opticalPumpingBiasfield, t0 - 10*us, 0) # turn off optical pumping bias field
event(ddsRfKnife, t0 + 1*ms, (180, 0, 0))



#### Make a mot ####
time = t0

setvar('varCMOTCurrent', 40)

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 40*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 4.25, dtCMOT = 11*ms, powerReduction = 1.0, CMOTFrequency = 210, dtNoRepump = 10*ms, repumpAttenuatorVoltage = 0)

#210

# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)

if (opticallyPump) :
#    time = depumpMOT(time + 1*us, pumpingTime = 1000*us)
    time = time + 10*us
else :
    time = time + 10*us




#

if (magneticTrap) :
    setvar('varFullMagneticTrapCurrent', 40)
    setvar('varChargeCurrent', 40)
    setvar('varDischargeCurrent', 40)
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


##Image
dtDeadMOT = 100*ms

if(realTime) : 
         ## Take an absorbtion image using Andor Solis Software ##
    if(fluorescenceImage) :
        time = takeSolisSoftwareFluorescenceImage(time, dtFluorescenceExposure = 2*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 5)
        
    else :
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
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False, motQuadCoilCurrent = 4.25)    # turn MOT back on

#event(ch(digitalOut, 4), time + 4*s, 0)








