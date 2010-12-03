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
include('repumpFunction.py')

setvar('imageCropVector',(500, 500, 490))
setvar('dtDriftTime', 0.4000*ms)

setvar('MOTLoadTime', 1500*ms )

setvar('desc',"simple absorption image")

# Global definitions


t0 = 2*ms





event(starkShiftingAOM, 100*us, starkShiftOff)
event(probeLightAOM, t0, probeLightOff)             # AOM is off, so no imaging light


#### Make a mot ####
time = t0

time = MOT(time, tClearTime=100*ms, cMOT=True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT=10*ms, cmotQuadCoilCurrent = 7, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 7, dtCMOT = 1*ms, powerReduction = 1, CMOTFrequency = 180)




#### Drift ###
time = time + dtDriftTime

#### repump out of F = 1' #####
time = repumpMOT(time, pumpingTime = 500*us)



##Image
dtDeadMOT = 200*ms

setvar('realTime', False)

if(realTime) : 
     ## Take an absorbtion image using Andor Solis Software ##

    dtAbsorbtionLight = 50*us
    dtCameraDelay = 5*us
    expTime = 100*us

    tAOM  = time - dtAOMHoldoff
    tCameraTrigger = time - dtCameraDelay

    event(probeLightAOM, tAOM, probeLightOn)               #turn on absorbtion light
    event(probeLightAOM, tAOM + dtAbsorbtionLight, probeLightOff)              #turn off absorbtion light

    event(cameraTrigger, tCameraTrigger, 1)
    event(cameraTrigger, tCameraTrigger + expTime, 0)
else : 
    ### Andor Camera ###
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    print time
    time = takeAbsorptionImage(time, time + dtDeadMOT, cropVector=imageCropVector)


    ## Turn on MOT steady state

tTAEndOfSequence = time +2*ms
time = MOT(tTAEndOfSequence, leaveOn=True, cMOT = False)    # turn MOT back on