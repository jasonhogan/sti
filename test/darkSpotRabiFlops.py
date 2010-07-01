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

setvar('MOTLoadTime', 250*ms )

setvar('desc',"Determining rabi frequency for 2 photon raman transistion")

# Global definitions


t0 = 2*ms

# digital trigger
#event(ch(digitalOut, 4), t0, 1)
#event(ch(digitalOut, 4), t0 + 10*us, 0)



event(starkShiftingAOM, 100*us, starkShiftOff)
event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light
event(sixPointEightGHzSwitch, t0 - 1*ms, 0)                    # turn off microwave horn, so no microwave transitions are applied


#### Make a mot ####
time = t0
time = MOT(time, tClearTime=100*us, cMOT=True, dtMOTLoad=MOTLoadTime, dtCMOT=20*ms)
motFinishedLoading=time


#### pump the MOT into F = 1 ####
setvar('depumpTime', 15*us)

event(repumpFrequencySwitchX, motFinishedLoading - depumpTime, 1)
event(motFrequencySwitch, motFinishedLoading - depumpTime, 1)
event(repumpFrequencySwitchX, motFinishedLoading, 0)
event(motFrequencySwitch, motFinishedLoading, 0)

#### Turn on dark spot (post depump) ####
setvar('darkSpotOn', False)
setvar('darkSpotDelay', 10*us)

if(darkSpotOn) :
    event(starkShiftingAOM, motFinishedLoading, starkShiftOn)    # ON
else :
    event(starkShiftingAOM, motFinishedLoading, starkShiftOff)

#### Turn MOT back on ####
#time = MOT(motFinishedLoading + darkSpotDelay, leaveOn=True, cMOT = False)    # turn MOT back on


#### Apply a rabi flop pulse ####
ramseyStartTime = motFinishedLoading + dtDriftTime

#setvar('dtRabiSequence', 1000*us)
#setvar('dtRabiPulse', dtRabiSequence)
setvar('dtRabiPulse', 500*us)

event(sixPointEightGHzSwitch, ramseyStartTime, 1)
event(sixPointEightGHzSwitch, ramseyStartTime + dtRabiPulse, 0)

time = ramseyStartTime + dtRabiPulse + dtDriftTime

print time

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
    time = takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)




event(starkShiftingAOM, time + 1*ms, starkShiftOff)                       # turn off dark spot
#event(probeLightAOM, time, (probeAOMFreq, 100, 0) )               #turn on absorbtion light

