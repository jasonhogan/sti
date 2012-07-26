from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

include('channels.py')
include('motFunction.py')
include('andorCameraFunctions.py')

setvar('desc',"MOT lifetime")
setvar('imageCropVector',(500, 500, 490))

exposureList=[]



time = 10*ms

## Take an Fluorescence image using Andor Device ##

andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
camera = ch(andorCamera, 0)
        
setvar('dtImageDelay', 5*s)

numImages = 50


for i in range(1, numImages + 1) :
    if(i < 35) :
        scale = 1
    elif(i < 40) :
        scale = 2
    elif(i < 45) :
        scale = 3
    else :
        scale = 4
    exposureList.append(5*ms*scale)
    time = takeFluorescenceImage(time + dtImageDelay, exposureList[-1], leaveMOTLightOn=True, cropVector=imageCropVector, indexImages=True, imageIndex=i)


setvar('dtFluorescenceExposure', exposureList)
