from stipy import *
include('headers.py')

setvar('desc', "5*s load 8A 180 MHz cMOT, 1*ms sweep -> 20*ms hold, 01*ms drift")

#setvar('imageCropVector',(514, 393 ,250))
setvar('imageCropVector',(500, 500, 490))

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', .01*ms)

#setvar('holdTimeSequence', 1*s)
#setvar('holdTime', holdTimeSequence)
setvar('holdTime', 1000*ms)

setvar('MOTLoadTime', 1*s )

setvar('usePreCharge', False)

setvar('realTime', True)

# Global definitions

t0 = 2*ms

#setvar('deltaImagingFreqSequence', 10e+06)
setvar('deltaImagingFreq', 10e+06)

event(imagingOffsetFrequency, t0, 1.8e+09 + deltaImagingFreq)

event(probeLightRFSwitch, t0, probeLightOff)             # AOM is off, so no imaging ligh
event(probeLightShutter, t0+1*ms, 0)

event(capacitorPreChargeRelay, t0, 5)
event(capacitorPreChargeRelay, t0 + 1*s, 0)
event(preChargeVoltage, t0-10*us, 0.75) # set cap voltage to 45V using Lambda

#### Make a mot ####
time = t0 + 100*ms

setvar('varCMOTCurrent', 8)
setvar('chargeTime', 10*ms)

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 1*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 8, dtCMOT = 20*ms, powerReduction = 1.0, CMOTFrequency = 180, dtNoRepump = 5*ms, repumpAttenuatorVoltage = 0, cmotCurrentRampRate = 1)

# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)

time = depumpMOT(time + 10*us, pumpingTime = 100*us)

if (usePreCharge) :
    preChargeTime = time - 10*ms
    event(quadrupoleChargeSwitch, preChargeTime, 1)
    setQuadrupoleCurrent(time + chargeTime - 5*ms, 40, applyCurrentRamp = False, usePrecharge = False, startingCurrent = 0)
    event(quadrupoleChargeSwitch, preChargeTime + chargeTime, 0) # was 5*ms before we started monkeying with it (i.e. for 45A fast turn on)
    time = preChargeTime + chargeTime

time = setQuadrupoleCurrent(time, desiredCurrent = 35, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 40, rampRate = 1)


#### Drift ###

time = time + holdTime

tOff = time
setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
event(quadrupoleOnSwitch, tOff, 0)

time = time + dtDriftTime

time = repumpMOT(time + 10*us, pumpingTime = 300*us) 


##Image
dtDeadMOT = 200*ms

if(realTime) : 
         ## Take an absorbtion image using Andor Solis Software ##
    time = takeSolisSoftwareAbsorptionImage (time, 75*us, dtAbsorbtionLight = 25*us)
    
else : 
        ### Andor Camera ###
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    print time
    time = takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)

      
    ## Turn on MOT steady state

tTAEndOfSequence = time +2*ms
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False)    # turn MOT back on

#event(ch(digitalOut, 4), time + 4*s, 0)










