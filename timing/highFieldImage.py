from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

include('channels.py')
include('experimentalParameters.py')
include('makeCMOTFunction.py')
include('repumpFunction.py')
include('depumpFunction.py')
include('andorCameraFunctions.py')

setvar('desc', "High field image")

#setvar('holdTimeSequence', 1*s)
#setvar('holdTime', holdTimeSequence)
setvar('holdTime', 0.1*s)

setvar('dtHoldInF2state', 0*ms)

setvar('imageCropVector',(500, 500, 490))

setvar('opticallyPump', True)
setvar('rampToMaxCurrent', True)
setvar('realTime', True)


######## Prepare atom cloud ########

tStart = 100*us

time = tStart
time = makeCMOT(time)
time = turnMOTLightOff(time)
time = depumpMOT(time + 10*us, pumpingTime = 100*us)


#setvar('varFullMagneticTrapCurrent', 275)
#setvar('varDischargeCurrent', 35) 

if (rampToMaxCurrent) :
    time = rampUpQuadCoils(time, True, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, chargeCurrent = CMOTcurrent)

time += holdTime


## repump out of F = 1'  (***Caution: In high field the efficiency is not spatially uniform.)

if (opticallyPump) :
    time = repumpMOT(time + 10*us, pumpingTime = 100*us)

time += dtHoldInF2state

## Take high field image

if(realTime) : 
    # Take an absorbtion image using Andor Solis Software
    time = takeSolisSoftwareAbsorptionImage (time, expTime = 75*us, dtAbsorbtionLight = 25*us, iDus = True)
#    takeSolisSoftwareFluorescenceImage(time+100*us, dtFluorescenceExposure = 3*ms, leaveMOTLightOn = False, iDusImage = True, iXonImage = True, imagingDetuning = 0)

else : 
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    imageTime = time
    dtDeadMOT = 500*ms
    time = takeAbsorptionImage(imageTime, imageTime + dtDeadMOT, cropVector=imageCropVector)
#    takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 100*us, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)


if (rampToMaxCurrent) :
    time = rampDownQuadCoils(time, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, dischargeCurrent = varDischargeCurrent, rapidOff = False)


event(digitalSynch,time-10*ms,0) #trigger webscope for quad coil ramp
event(digitalSynch,time,1) 
event(digitalSynch,time+10*ms,0) 


### Snap off magnetic field
time = rampDownQuadCoils(time, rapidOff = True)



###############################################################
    

      
### Turn on MOT steady state ##################################################

MOT(time + 150*ms, leaveOn=True, cMOT = False)    # turn MOT back on


