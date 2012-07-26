from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

#setvar('cmotBoost',12)

include('channels.py')
include('experimentalParameters.py')
include('makeCMOTFunction.py')
include('repumpFunction.py')
include('depumpFunction.py')
include('andorCameraFunctions.py')


#setvar('holdTimeSequence', 0.0001*s)
#setvar('holdTime', holdTimeSequence)
setvar('holdTime', 300*ms) #300

#setvar('imageCropVector',(500, 500, 490))
setvar('imageCropVector',(1,60,1500,1500))

setvar('realTime', True)

setvar('desc', "Test imaging beam only")

######## Prepare atom cloud ########

tStart = 100*us

time = tStart

time = turnMOTLightOff(time)

time += holdTime


######## Measure the result: Snap off field, repump, and image ########

### Image

if(realTime) : 
    # Take an absorbtion image using Andor Solis Software
    time = takeSolisSoftwareAbsorptionImage (time, expTime = 300*us, dtAbsorbtionLight = 50*us, iDus = False)
#    takeSolisSoftwareFluorescenceImage(time+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, iXonImage = True, imagingDetuning = 0)

else : 
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    imageTime = time
    dtDeadMOT = 500*ms
    depumpMOT(imageTime-100*us, pumpingTime = 1000*us)
    time = takeAbsorptionImage(imageTime, imageTime + dtDeadMOT, cropVector=imageCropVector)
#    takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)

###############################################################
    

      
### Turn on MOT steady state ##################################################

#MOT(time + 150*ms, leaveOn=True, cMOT = False)    # turn MOT back on
