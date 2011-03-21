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

setvar('rabiPulseTime', 100*us)
setvar('microwavePower', 7)

setvar('rbResonanceFreq', 6.834682610e+009 - 1e+003)
setvar('resFreq', rbResonanceFreq)

setvar('MOTLoadTime', 2*s )
setvar('opticallyPump', True)
setvar('realTime', False)

setvar('zBiasL', 0.67) 
setvar('zBiasR', 1.70)
setvar('yBiasL', 0.0)
setvar('yBiasR', 4.351) 
setvar('xBiasL', -1.9) 
setvar('xBiasR', 2.27)
setvar('topBias1', 0.06)
setvar('topBias2', -0.07)
setvar('quadrupoleCompensation', -7.61)

setvar('rfOn', True)
setvar('deltaFreq', 0e+003)


setvar('desc', "microwave spectroscopy - 1*s load, CMOT - 0 MHz detuned probe - 600*us scanning from 0 kHz to +60 kHz detuned MW - 100*us optical pumping - -6.48A in new quad trim coils, rapidOff = False")

# Global definitions


t0 = 200*ms

if(rfOn):
#    event(ch(hp83711b, 3), t0-180*ms, microwavePower)
    event(rfKnifeFrequency, t0 - 180*ms, resFreq + deltaFreq)


event(starkShiftingAOM, 100*us, (50,0,0))
event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light
event(opticalPumpingBiasfield, t0 - 10*us, 0) # turn off optical pumping bias field
event(ddsRfKnife, t0 + 1*ms, (180, 0, 0))



#### Make a mot ####
time = t0

setvar('varCMOTCurrent', 8)

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 1*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 8, dtCMOT = 10*ms, powerReduction = 0.7, CMOTFrequency = 180, dtNoRepump = 0*ms, repumpAttenuatorVoltage = 0)

#210

if (opticallyPump) :
    time = depumpMOT(time + 1*us, pumpingTime = 100*us)


# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)


#### Drift ###
time = time + dtDriftTime


### RF

if (rfOn) :
    dtCut = rabiPulseTime
    event(sixPointEightGHzSwitch, time, 1)
    event(sixPointEightGHzSwitch, time + dtCut, 0)
    time = time + dtCut


##Image
dtDeadMOT = 100*ms



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








