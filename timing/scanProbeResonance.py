from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

include('channels.py')
include('makeCMOTFunction.py')
include('repumpFunction.py')
include('andorCameraFunctions.py')

setvar('desc', "CMOT test")

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 0.1*ms)

#setvar('imageCropVector',(500, 500, 490))
setvar('imageCropVector',(1,60,1500,1500))

setvar ('opticallyPump', True)
setvar('realTime', False)

f0 = 4609*1e6 #1802 for 2>3', 4766 for 1>2', 4609 for 1>1 

#setvar('deltaFSeq', 0)
#setvar('deltaF', deltaFSeq)
setvar('deltaF', 0)

######## Prepare atom cloud ########

tStart = 100*us


time = tStart

event(imagingOffsetFrequency, time, f0 + (deltaF * 1e6))
time+=1000*ms    ##make sure the frequency is set

time = makeCMOT(time)
time = turnMOTLightOff(time)


######## Measure the result: Snap off field, repump, and image ########

### Snap off magnetic field
time = rampDownQuadCoils(time, rapidOff = True)

### repump out of F = 1'
if (opticallyPump) :
    time = repumpMOT(time + 10*us, pumpingTime = 100*us)

### Image

time += dtDriftTime

if(realTime) : 
    # Take an absorbtion image using Andor Solis Software
    time = takeSolisSoftwareAbsorptionImage (time, expTime = 75*us, dtAbsorbtionLight = 25*us, iDus = True)

else : 
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    imageTime = time
    dtDeadMOT = 500*ms
    time = takeAbsorptionImage(imageTime, imageTime + dtDeadMOT, cropVector=imageCropVector)
#    takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)


###############################################################
    

      
### Turn on MOT steady state ##################################################

MOT(time + 150*ms, leaveOn=True, cMOT = False)    # turn MOT back on


