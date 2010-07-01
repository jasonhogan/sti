from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

include('channels.py')
include('motFunction.py')
include('absorptionImageFunction.py')






setvar('imageCropVector',(495, 660 ,300))
#setvar('imageCropVector',(500, 500 ,480))


setvar('dtDriftTime', 10*us)

setvar('MOTLoadTime', 400*ms )

setvar('desc',"Determining rabi frequency for 2 photon raman transistion")

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
time = MOT(time, tClearTime=100*ms, cMOT=True, dtMOTLoad=MOTLoadTime, dtCMOT=20*ms)
motFinishedLoading=time


#### pump the MOT into F = 1 ####
setvar('depumpTime', 15*us)

event(repumpFrequencySwitchX, motFinishedLoading - depumpTime, 1)
event(motFrequencySwitch, motFinishedLoading - depumpTime, 1)
event(repumpFrequencySwitchX, motFinishedLoading, 0)
event(motFrequencySwitch, motFinishedLoading, 0)

#### Apply a pi/2 pulse ####

ramseyStartTime = motFinishedLoading + dtDriftTime


setvar('pi2pulse', 550*us)

event(sixPointEightGHzSwitch, ramseyStartTime, 1)
event(sixPointEightGHzSwitch, ramseyStartTime + pi2pulse, 0)

time = ramseyStartTime + pi2pulse

print time

#### wait for time T then apply pi/2 pulse####

setvar('interferometerTimeSequence', 1000*us)
setvar('interferometerTime', interferometerTimeSequence)
#setvar('interferometerTime', 0.5*ms)

ramseyEndTime = time + interferometerTime

event(sixPointEightGHzSwitch, ramseyEndTime, 1)
event(sixPointEightGHzSwitch, ramseyEndTime + pi2pulse, 0)

time = ramseyEndTime + pi2pulse

print time

##Image
dtDeadMOT = 100*ms

setvar('realTime', False)

if(realTime) : 
     ## send an external trigger to the camera ##

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
    time = takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)


    ## Turn on MOT steady state
tTAEndOfSequence = time +2*ms
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False)    # turn MOT back on

#event(probeLightAOM, time, (probeAOMFreq, 100, 0) )               #turn on absorbtion light

