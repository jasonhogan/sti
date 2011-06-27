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

setvar('desc', "CMOT test; MOT load time " + str(MOTLoadTime/s) + " s")

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 2*ms)

#setvar('imageCropVector',(500, 500, 490))
setvar('imageCropVector',(1,60,1500,1500))

setvar ('opticallyPump', True)
setvar('realTime', False)

#setvar('deltaImagingFreqMHz', 0)

######## Prepare atom cloud ########

tStart = 100*us
#event(imagingOffsetFrequency, tStart, imagingResonanceFreq + deltaImagingFreqMHz*1e6)
time = makeCMOT(tStart)
time = turnMOTLightOff(time)


######## Measure the result: Snap off field, repump, and image ########

### Snap off magnetic field
time = rampDownQuadCoils(time, rapidOff = True)

### repump out of F = 1'
if (opticallyPump) :
    time = repumpMOT(time + 10*us, pumpingTime = 100*us)

#time = depumpMOT(time + 10*us, pumpingTime = 100*us)    #10ms

### Image

time += dtDriftTime

if(realTime) : 
    # Take an absorbtion image using Andor Solis Software
    if(Fis1Imaging) :
        time = takeSolisSoftwareAbsorptionImage (time, expTime = 75*us, dtAbsorbtionLight = 10*us, iDus = True)    #expTime = 10*75*us, dtAbsorbtionLight = 10*25*us
    else:
        time = takeSolisSoftwareAbsorptionImage (time, expTime = 20*us, dtAbsorbtionLight = 10*us, iDus = True)

else : 
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    imageTime = time
    dtDeadMOT = 500*ms
    time = takeAbsorptionImage(imageTime, imageTime + dtDeadMOT, cropVector=imageCropVector)
#    takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)

 #   takeFluorescenceImage(imageTime, dtFluorescenceExposure = 1*ms, cropVector = imageCropVector, repumpOn = False)

###############################################################
    

      
### Turn on MOT steady state ##################################################

MOT(time + 150*ms, leaveOn=True, cMOT = False)    # turn MOT back on


