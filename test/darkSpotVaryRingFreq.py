from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

include('channels.py')
include('motFunction.py')
include('absorptionImageFunction.py')


### Andor Camera ###
andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
camera = ch(andorCamera, 0)



#setvar('imageCropVector',(497, 489 ,200))
setvar('imageCropVector',(500, 500 ,480))

setvar('darkSpotDriftTime',300*us)
#setvar('dtDriftTime', darkSpotDriftTime)
setvar('dtDriftTime', 15*us + darkSpotDriftTime)

setvar('trapHoldTimeSeq', 10*ms)
setvar('trapHoldTime', trapHoldTimeSeq)


#setvar('MOTLoadTime', 500*ms )
setvar('MOTLoadTime', 500*ms + trapHoldTime)


# Global definitions


t0 = 11*us + 2*ms

# digital trigger
event(ch(digitalOut, 4), t0, 1)
event(ch(digitalOut, 4), t0 + 10*us, 0)

meas(ch(wavemeter, 0), t0, "Measure 1530 frequency")
meas(absoptionLightFrequency, t0)

event(starkShiftingAOM, 100*us, starkShiftOff)
event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light

time = t0
time = MOT(time, tClearTime=100*ms, cMOT=False, dtMOTLoad=MOTLoadTime, dtCMOT=20*ms)
motFinishedLoading=time


##1530 experiment

setvar('darkSpotOn', True)
setvar('depumpMOT', True)


setvar('depumpTime', 5*us + 5*us + trapHoldTime)

setvar('irHoldoffSeq',10*us)
setvar('irHoldoff', 100*us)

if(darkSpotOn) :
    setvar('desc',"Trapped atoms in dark spot--1530 On -- fixed hold time varying TA2 voltage, TA3 off")
#    event(starkShiftingAOM, motFinishedLoading - depumpTime - irHoldoff, starkShiftOn)    # OFF during load
    event(starkShiftingAOM, t0+50*ms, starkShiftOn)    # ON during load
    event(starkShiftingAOM, motFinishedLoading, starkShiftOff)
else :
    setvar('desc',"Atoms in dark spot--1530 Off")
    event(starkShiftingAOM, t0+50*ms, starkShiftOff)
    event(starkShiftingAOM, motFinishedLoading, starkShiftOff)


#depump the MOT

setvar('TA2voltageSeq',1.1)
setvar('TA2voltage', TA2voltageSeq)

if(depumpMOT) :
    event(quadCoilSwitch, motFinishedLoading - depumpTime, 0)                             # turn off the quad coils
    event(TA3, motFinishedLoading - depumpTime, 0)             # TA3 off during depump/trap
    event(TA2, motFinishedLoading - depumpTime + 15*us, TA2voltage)             # Turn TA2 down
    event(repumpFrequencySwitchX, motFinishedLoading - depumpTime, 1)
    event(motFrequencySwitch, motFinishedLoading - depumpTime, 1)
    event(repumpFrequencySwitchX, motFinishedLoading, 0)
    event(motFrequencySwitch, motFinishedLoading, 0)

print time

##Image
dtDeadMOT = 100*ms
time = takeAbsorptionImage(time + dtDriftTime, time + dtDriftTime + dtDeadMOT, cropVector=imageCropVector)


## Turn on MOT steady state

tTAEndOfSequence = time +2*ms
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False)    # turn MOT back on

