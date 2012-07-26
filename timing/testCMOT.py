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
include('raman.py')

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 0.1*ms)

setvar('desc', "CMOT test; MOT load time " + str(MOTLoadTime/s) + " s; DriftTime " + str(dtDriftTime/ms) + " ms; Det " + str(deltaImagingFreqMHz) +" MHz.")

##setvar('imageCropVector',(500, 500, 490))
setvar('imageCropVector',(1, 60+30, 1003-50, 913-50))

setvar ('opticallyRepump', True)
setvar('opticallyDepump', False)

setvar('realTime', False)
setvar('depumpImage', False)
setvar('absorptionImage', True)

######## Prepare atom cloud ########

tStart = 100*us+5*ms
#event(imagingOffsetFrequency, tStart, imagingResonanceFreq + deltaImagingFreqMHz*1e6)

#meas(masterLockCheck, tStart, "Master lock check at start of sequence.")
#meas(imagingLockCheck, tStart, "Imaging lock check at start of sequence.")

ramanModulationOff(tStart - 0.1*ms)

##########################
######## Safety ##########
##########################

event(topSafetySwitch, tStart -1*ms + 10*us, 0)
event(topVCA, tStart -1*ms + 10*us, 0)

event(imagingDetuning, tStart, 80 + deltaImagingFreqMHz)
time = makeCMOT(tStart)

#timeFluorescenceExposure = time-.05*s

time = turnMOTLightOff(time)
#event(TA8, time-2*ms, 0)

#
#time += 10.4*ms
#pulse = 100*us
##event(TA7, time-0.05*ms, ta7MotVoltage)                             # z-Axis seed TA on
#event(zAxisRfSwitch, time, 1)
#event(TA4, time, ta4MotVoltage)
#time +=pulse
##event(TA7, time, ta7OffVoltage)                             # z-Axis seed TA on
#event(TA4, time, ta4OffVoltage)     
#time += 10*us



time += 0.8*ms



setvar('depumpTime', 70*us)  #70*us

if (opticallyDepump) :
    time = depumpMOT(time + 10*us, pumpingTime = depumpTime)    #10ms
    time += 10*us

######## Measure the result: Snap off field, repump, and image ########

### Snap off magnetic field
#time = rampDownQuadCoils(time, rapidOff = True)
snapOffField(time)

time += 700*us               #allow fields to shut off; leave time for repump pulse


### Image

time += dtDriftTime


setvar('repumpTime', 200*us)    #1000*us #25*us with new cooling shutter


### repump out of F = 1'
if (opticallyRepump) :
    time = repumpMOT(time + 10*us, pumpingTime = repumpTime)
    time += 10*us
else :
    time += 120*us

triggerTime = time

if(realTime) : 
    # Take an absorbtion image using Andor Solis Software
    if(Fis1Imaging) :
        time = takeSolisSoftwareAbsorptionImage (time, expTime = 75*us, dtAbsorbtionLight = 25*us, iDus = True)    #expTime = 10*75*us, dtAbsorbtionLight = 10*25*us
    else:
        time = takeSolisSoftwareAbsorptionImage (time, expTime = 50*us, dtAbsorbtionLight = 35*us, iDus = False)
#        time = takeSolisSoftwareFluorescenceImage(time+10*us, dtFluorescenceExposure = 10*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)
    
else : 
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    imageTime = time
    dtDeadMOT = 500*ms
    if (absorptionImage):
        time = takeAbsorptionImage(imageTime, imageTime + dtDeadMOT, cropVector=imageCropVector, depumpAbsImage = depumpImage)
#        takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)
#        takeFluorescenceExposure(timeFluorescenceExposure, dtFluorescenceExposure=1*ms, cropVector = imageCropVector)
    else:
       time = takeFluorescenceImage(imageTime, dtFluorescenceExposure = 10*ms, cropVector = imageCropVector, repumpOn = True)



###############################################################
    

      
### Turn on MOT steady state ##################################################

tSteadStateMot = time + 150*ms

MOT(tSteadStateMot, leaveOn=True, cMOT = False, motQuadCoilCurrent = MOTcurrent)    # turn MOT back on
#event(digitalSynch, time+1*s,0) 


event(digitalSynch, triggerTime - 10*ms,0) #trigger webscope for quad coil ramp
event(digitalSynch, triggerTime,1) 
event(digitalSynch, triggerTime + 10*ms,0)

#meas(masterLockCheck, tSteadStateMot, "Master lock check at end of sequence.")
#meas(imagingLockCheck, tSteadStateMot, "Imaging lock check at end of sequence.")

