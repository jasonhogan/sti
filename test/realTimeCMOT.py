from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc',"Real time feedback CMOT")

include('channels.py')
include('motFunction.py')
include('absorptionImageFunction.py')


setvar('MOTLoadTime', 200*ms)
setvar('MOTOffTime', 200*ms)

setvar('cmotTime', 20*ms)
setvar('dtDriftTime', 1*ms)
setvar('depumpMOT',True)

cameraTrigger=ch(digitalOut,0)

t0 = 11*us + 100*ms

dtAbsorbtionLight = 50*us
dtCameraDelay = 5*us
expTime = 100*us

num=25
dt=10*us    # initially; this gets automatically set inside the for loop

for i in range(0, num) :

    # digital trigger
    event(ch(digitalOut, 4), t0 + i*dt, 1)
    event(ch(digitalOut, 4), t0 + (i+0.5)*dt, 0)

    ## Load the MOT ##    
    tDoneLoading = MOT(t0 + i*dt, tClearTime=0, cMOT=False, dtMOTLoad=MOTLoadTime, dtCMOT=cmotTime)


    #depump the MOT
    depumpTime = 3*us

    if(depumpMOT) :
        event(repumpFrequencySwitch, tDoneLoading - depumpTime, 1)
        event(motFrequencySwitch, tDoneLoading - depumpTime, 1)
        event(repumpFrequencySwitch, tDoneLoading, 0)
        event(motFrequencySwitch, tDoneLoading, 0)

    ## Take an absorbtion image ##
    tAOM                 = tDoneLoading + dtDriftTime - dtAOMHoldoff
    tCameraTrigger = tDoneLoading + dtDriftTime - dtCameraDelay

    event(probeLightAOM, tAOM, probeLightOn)               #turn on absorbtion light
    event(probeLightAOM, tAOM + dtAbsorbtionLight, probeLightOff)              #turn off absorbtion light

    event(cameraTrigger, tCameraTrigger, 1)
    event(cameraTrigger, tCameraTrigger + expTime, 0)

    ## MOT Blow Away
    tBlowAway = tCameraTrigger + expTime + 1*ms

    event(motFrequencySwitch, tBlowAway, 1)
    event(TA2, tBlowAway, 1.4)
    event(TA3, tBlowAway, 1.4)

    ##MOT Off
    MOT(tBlowAway+1*ms, dtMOTLoad = 0)

    dt = (tBlowAway+1*ms+MOTOffTime) - (t0 + i*dt)





### Turn on MOT steady state
#tTAEndOfSequence = t0 + num*dt +200*ms
#time = MOT(tTAEndOfSequence, leaveOn=True)    # turn MOT back on

