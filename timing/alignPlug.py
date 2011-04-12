from stipy import *

include('channels.py')
include('motFunction.py')
include('andorCameraFunctions.py')
include('repumpFunction.py')
include('depumpFunction.py')
include('microwaveKnifeFunction.py')

setvar('desc', "Simple absorbtion image - MOT - 1800 MHz probe - with depump pulse + 100*us MW repump scanning frequency - +200 mV on TOP 1")

#setvar('imageCropVector',(514, 393 ,250))
setvar('imageCropVector',(500, 500, 490))

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 5*ms)

setvar('MOTLoadTime', 0.1*s )
setvar('deltaFreq', 0.15)
setvar('dtRabiPulseTime', 100*us)

#setvar('topBias1Sequence', 0.0)

setvar('zBiasL', 1.35) 
setvar('zBiasR', 4.7)
setvar('yBiasL', 1.4)
setvar('yBiasR', 3.4) 
setvar('xBiasL', -4.8) 
setvar('xBiasR', 1.3)
setvar('topBias1', 0.2)
setvar('topBias2', 0)


setvar('realTime', True)

# Global definitions

t0 = 2*ms


event(probeLightRFSwitch, t0, probeLightOff)             # AOM is off, so no imaging ligh
event(probeLightShutter, t0+1*ms, 0)

#event(opticalPumpingBiasfield, t0 - 10*us, 0) # turn off optical pumping bias field




#### Make a mot ####
time = t0 + 100*ms
event(ch(slowAnalogOut, 10), time - 1*ms, topBias1)
event(ch(slowAnalogOut, 11), time + 1*ms, topBias2)

event(ddsRfKnife, time - 100*us, (ddsRbResonanceFreq + deltaFreq, 100, 0))

setvar('varCMOTCurrent', 0)    #8

time = MOT(time, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 1*ms, cmotQuadCoilCurrent = varCMOTCurrent, dtMolasses = 0*ms, rapidOff = True, motQuadCoilCurrent = 8, dtCMOT = 20*ms, powerReduction = 1.0, CMOTFrequency = 180, dtNoRepump = 0*ms, repumpAttenuatorVoltage = 0)

#time = depumpMOT(time + 10*us, pumpingTime = 100*us)
#time = time + 1*ms

# digital trigger
event(ch(digitalOut, 4), time - 500*us, 1)
event(ch(digitalOut, 4), time + 1*ms, 0)

#### Drift ###

time = time + dtDriftTime

#time = repumpMOT(time + 10*us, pumpingTime = 100*us)

#### RF
#rabiPulseTime = 50*us
#event(rfKnifeAmplitude, time - 100*us, 6.75)
#
#event(sixPointEightGHzSwitch, time, 1)
#event(sixPointEightGHzSwitch, time + dtRabiPulseTime, 0)
#time = time + dtRabiPulseTime

##Image
dtDeadMOT = 100*ms

if(realTime) : 
         ## Take an absorbtion image using Andor Solis Software ##
#    time = takeSolisSoftwareAbsorptionImage (time, 75*us, dtAbsorbtionLight = 25*us)    
    time = takeSolisSoftwareFluorescenceImage(time+10*us, dtFluorescenceExposure = 2*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 20)

else : 
        ### Andor Camera ###
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    print time
    time = takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)
    time = takeSolisSoftwareFluorescenceImage(time+10*us, dtFluorescenceExposure = 10*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 10)
      
    ## Turn on MOT steady state

tTAEndOfSequence = time +2*ms
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False)    # turn MOT back on

#event(ch(digitalOut, 4), time + 4*s, 0)








