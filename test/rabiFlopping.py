from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

include('channels.py')
include('motFunction.py')
include('absorptionImageFunction.py')
include('evaporativeCoolingFunction.py')






#setvar('imageCropVector',(551, 375 ,250))
setvar('imageCropVector',(500, 500 ,499))

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime',1*ms)



setvar('dtMagneticTrapSequence', 1000*us)
setvar('dtMagneticTrap', dtMagneticTrapSequence)
#setvar('dtMagneticTrap',1*ms)

setvar('MOTLoadTime', 1000*ms )

setvar('desc',"working out cooling sequence for large CMOT")

# Global definitions


t0 = 2*ms

# digital trigger
event(ch(digitalOut, 4), t0, 1)
event(ch(digitalOut, 4), t0 + 10*us, 0)



event(starkShiftingAOM, 100*us, starkShiftOff)
event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light
event(sixPointEightGHzSwitch, t0, 0)                    # turn off microwave horn, so no microwave transitions are applied


#### Make a mot ####
time = t0
time = MOT(time, tClearTime=100*ms, cMOT=True, dtMOTLoad=MOTLoadTime, dtCMOT=20*ms, cmotFieldMultiplier = 3.5, dtMolasses = 1*us, rapidOff=True, quadCoilSetting = 1.8, dtFarDetuned = 80*ms)

#### pump the MOT into F = 1 ####
setvar('depumpTime', 30*us)

event(repumpFrequencySwitchX, time - depumpTime, 1)
event(motFrequencySwitch, time - depumpTime, 1)
event(repumpFrequencySwitchX, time, 0)
event(motFrequencySwitch, time, 0)

#### Hold in a magnetic Trap ####


#time = evaporate(time, 1*s, magneticTrapSetpoint = 6, rapidOff = False)
#time = evaporate(time, dtMagneticTrap)

#### Drift ###
#time = time + dtDriftTime

### Turn on dark spot (post depump) ####
#setvar('darkSpotOn', False)
#setvar('darkSpotDelay', 10*us)
#
#if(darkSpotOn) :
#    event(starkShiftingAOM, motFinishedLoading + darkSpotDelay, starkShiftOn)    # ON
#else :
#    event(starkShiftingAOM, motFinishedLoading + darkSpotDelay, starkShiftOff)

##### Leave MOT beams on ####
#setvar('motON', False)
#
#motDelay = 10*us
#
#voltageTA2test = 0.5
#voltageTA3test = 0.5
#
#if(motON) :
#    ### set the detuning ###
#    event(repumpFrequencySwitchX, motFinishedLoading + motDelay, 1) ### turn off repump
##    event(largeCoolingDetuningSwitch, motFinishedLoading + motDelay, 1) ### detune the MOT to 1.1 GHz
#    event(TA2, motFinishedLoading  + darkSpotDelay + motDelay, voltageTA2test)                   # TA on
#    event(TA3, motFinishedLoading + darkSpotDelay + motDelay, voltageTA3test)                   # TA on
#    ramseyStartTime = motFinishedLoading + darkSpotDelay + motDelay + dtDriftTime
#else :
#    ramseyStartTime = motFinishedLoading + darkSpotDelay + dtDriftTime

#### Ramsey Interferometer ####
setvar('dtRamseySequence', 1*ms)
setvar('dtRamsey', dtRamseySequence)
#setvar('dtRamsey', 1*ms)

#### Apply a pi/2 rabi flop pulse ####

###ramseyStartTime = motFinishedLoading + dtDriftTime + darkSpotDelay
##
ramseyStartTime = time + 10*us

setvar('dtRabiSequence', 100*us)
setvar('dtRabiPulse', dtRabiSequence)
#setvar('dtRabiPulse', 250*us)

event(sixPointEightGHzSwitch, ramseyStartTime, 1)
event(sixPointEightGHzSwitch, ramseyStartTime + dtRabiPulse, 0)
#
##time = ramseyStartTime + dtRabiPulse + dtRamsey
time = ramseyStartTime + dtRabiPulse + dtDriftTime

#### Apply a pi/2 rabi flop pulse ####

#ramseyStartTime = time
#
#event(sixPointEightGHzSwitch, ramseyStartTime, 1)
#event(sixPointEightGHzSwitch, ramseyStartTime + dtRabiPulse, 0)
#
#time = ramseyStartTime + dtRabiPulse + dtDriftTime
#
#


print time

### depump MOT ###
#setvar('secondDepump', False)

#secondDepumpStartTime = time
#
#voltageTA2 = 1.45
#voltageTA3 = 1.5
#
#
#if(secondDepump) :
#    event(TA2, secondDepumpStartTime, voltageTA2)                   # TA on
#    event(TA3, secondDepumpStartTime, voltageTA3)                   # TA on
#    event(repumpFrequencySwitchX, secondDepumpStartTime, 1)
#    event(motFrequencySwitch, secondDepumpStartTime, 1)
#    event(repumpFrequencySwitchX, secondDepumpStartTime + depumpTime, 0)
#    event(motFrequencySwitch, secondDepumpStartTime + depumpTime, 0)
#    event(TA2, secondDepumpStartTime + depumpTime, 0)                   # TA off
#    event(TA3, secondDepumpStartTime + depumpTime, 0)                   # TA off
#    event(starkShiftingAOM, secondDepumpStartTime + depumpTime + darkSpotDelay, starkShiftOff)
#    time = secondDepumpStartTime + depumpTime + darkSpotDelay + dtDriftTime
#
#else :
##    event(TA2,  secondDepumpStartTime + dtDriftTime, 0)                   # TA off
##    event(TA3, secondDepumpStartTime + dtDriftTime, 0)                   # TA off
##    event(starkShiftingAOM, secondDepumpStartTime + dtDriftTime, starkShiftOff)
#    time = secondDepumpStartTime + dtDriftTime
    



##Image
dtDeadMOT = 100*ms

setvar('realTime', True)

if(realTime) : 
     ## Take an absorbtion image ##

    

    dtAbsorbtionLight = 50*us
    dtCameraDelay = 5*us
    expTime = 100*us

    tAOM                 = time - dtAOMHoldoff
    tCameraTrigger = time - dtCameraDelay

    event(probeLightAOM, tAOM, probeLightOn)               #turn on absorbtion light
    event(probeLightAOM, tAOM + dtAbsorbtionLight, probeLightOff)              #turn off absorbtion light

    event(cameraTrigger, tCameraTrigger, 1)
    event(cameraTrigger, tCameraTrigger + expTime, 0)
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

event(starkShiftingAOM, time + 1*ms, starkShiftOff)                       # turn off dark spot
event(largeCoolingDetuningSwitch, time + 1*ms, 0) ### detune the MOT to 1.1 GHz
#event(probeLightAOM, time, (probeAOMFreq, 100, 0) )               #turn on absorbtion light

