from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

include('channels.py')
include('pgMotFunction.py')
include('absorptionImageFunction.py')






#setvar('imageCropVector',(551, 375 ,250))
setvar('imageCropVector',(500, 500 ,499))

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime',1400*us)

setvar('MOTLoadTime', 500*ms )

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
time = MOT(time, dtMOTLoad=MOTLoadTime, leaveOn = False, tClearTime=100*ms, cMOT=True, dtCMOT=2*ms, molasses=False)
motFinishedLoading = time


##Image
dtDeadMOT = 100*ms

setvar('realTime', False)

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

