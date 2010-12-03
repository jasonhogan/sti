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


setvar('MOTLoadTime', 500*ms)
setvar('MOTOffTime', 100*ms)

setvar('cmotTime', 20*ms)
setvar('dtDriftTime', 100*us)
setvar('depumpMOT',False)

cameraTrigger=ch(digitalOut,0)

t0 = 11*us + 100*ms

dtAbsorbtionLight = 50*us
dtCameraDelay = 5*us
expTime = 100*us

num=20
dt=1000*us    # initially; this gets automatically set inside the for loop

for i in range(0, num) :

    # digital trigger
    event(ch(digitalOut, 4), t0 + i*dt, 1)
    event(ch(digitalOut, 4), t0 + (i+0.5)*dt, 0)

    ## Load the MOT ##    
#    event(probeLightAOM, t0 + i*dt, (probeAOMFreq, 0, 0) )               #turn off absorbtion light
    tDoneLoading = MOT(t0 + i*dt, tClearTime=1*ms, cMOT=False, dtMOTLoad=MOTLoadTime, dtSweepToCMOT=1*ms, cmotQuadCoilCurrent = 7, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 7, dtCMOT = 1*ms, powerReduction = 1, CMOTFrequency = 180)

    #depump the MOT
    depumpTime = 500*us

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
#    event(probeLightAOM, tBlowAway, (probeAOMFreq, 100, 0) )               #turn on absorbtion light

    event(motFrequencySwitch, tBlowAway, 1)
    event(TA2, tBlowAway, 1.4)
    event(TA3, tBlowAway, 1.4)

    ##MOT Off
    MOT(tBlowAway+1*ms, dtMOTLoad = 0, cMOT=False)

    dt = (tBlowAway+1*ms+MOTOffTime) - (t0 + i*dt)





### Turn on MOT steady state
tTAEndOfSequence = t0 + num*dt +200*ms
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT=False)    # turn MOT back on
#event(probeLightAOM, time, (probeAOMFreq, 100, 0) )               #turn on absorbtion light

