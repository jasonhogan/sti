from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

include('channels.py')
include('motFunction.py')
include('absorptionImageFunction.py')


setvar('imageCropVector',(500,500,480))
setvar('dtDriftTime', 1*ms)




# Global definitions

t0 = 11*us + 2*ms

meas(ch(wavemeter, 0), t0, "Measure 1530 frequency")
meas(absoptionLightFrequency, t0)

event(starkShiftingAOM, 100*us, starkShiftOff)
event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light

time = t0
time = MOT(time, tClearTime=100*ms, cMOT=False)
motFinishedLoading=time


##1530 experiment

setvar('darkSpotOn',True)
setvar('depumpMOT',True)


if(darkSpotOn) :
    setvar('desc',"Atoms in dark spot--1530 On")
    event(starkShiftingAOM, t0+50*ms, starkShiftOn)
    event(starkShiftingAOM, motFinishedLoading, starkShiftOff)
else :
    setvar('desc',"Atoms in dark spot--1530 Off")
    event(starkShiftingAOM, t0+50*ms, starkShiftOff)
    event(starkShiftingAOM, motFinishedLoading, starkShiftOff)


#depump the MOT
depumpTime = 5*ms

if(depumpMOT) :
    event(repumpFrequencySwitch, motFinishedLoading - depumpTime, 1)  
    event(motFrequencySwitch, motFinishedLoading - depumpTime, 1)               

##Image

dtDeadMOT = 100*ms
time = takeAbsorptionImage(time + dtDriftTime, time + dtDriftTime + dtDeadMOT)


## Turn on MOT steady state

tTAEndOfSequence = time +2*ms
time = MOT(tTAEndOfSequence, leaveOn=True)    # turn MOT back on

