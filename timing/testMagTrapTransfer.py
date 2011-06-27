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

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime',0.1*ms)


#setvar('holdTimeSequence', 0.0001*s)
#setvar('holdTime', holdTimeSequence)
setvar('holdTime', 300*ms) #300  #7.225*ms

#setvar('imageCropVector',(500, 500, 490))
setvar('imageCropVector',(1,60,1500,1500))

setvar('opticallyPump', True)
setvar('rampToMaxCurrent', False)
setvar('realTime', False)
setvar('depumpImage', False)

setvar('desc', "Transfer into magnetic trap; Hold time " + str(holdTime/s) + " s; MOT load time " + str(MOTLoadTime/s) + " s")

setvar('depumpTime', 100*us)

#setvar('deltaImagingFreqMHzSeq', 0)
#setvar('deltaImagingFreqMHz', deltaImagingFreqMHzSeq)

######## Prepare atom cloud ########

tStart = 100*us

time = tStart

#event(imagingOffsetFrequency, tStart, imagingResonanceFreq + deltaImagingFreqMHz*1e6)                       ###COMMENTED SMD

time = makeCMOT(time)
time = turnMOTLightOff(time)

#event(motFrequencySwitch, (time + 10*us)+100*us, 1) # turn off all cooling modulation
#event(depumpSwitch, (time + 10*us)+100*us, 0) # turn off depump

time = depumpMOT(time + 10*us, pumpingTime = 100*us)    #100*us

tiggerTime= time
event(digitalSynch, tiggerTime-10*ms,0) #trigger webscope for quad coil ramp                                                       ###UNCOMMENTED BLOCK SMD
event(digitalSynch, tiggerTime,1) 
event(digitalSynch, tiggerTime+10*ms,0) 


#setvar('varFullMagneticTrapCurrent', 275) moved to experimentalParameters.py
#setvar('varDischargeCurrent', 35) moved to experimentalParameters.py

if (rampToMaxCurrent) :
#    time = rampQuadrupoleCurrent(startTime = time, endTime = time + 110*ms, startCurrent = CMOTcurrent, endCurrent = varFullMagneticTrapCurrent, numberOfSteps = 130)    #254*ms
    time = rampUpQuadCoils(time, True, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, chargeCurrent = CMOTcurrent)

time += holdTime

if (rampToMaxCurrent) :
    time = rampDownQuadCoils(time, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, dischargeCurrent = varDischargeCurrent, rapidOff = False)

#event(sfaRemoteCurrentSetVoltage, time, 0)    #"Rapid" off

if (rampToMaxCurrent) :
    ## Wait for eddys to die down
    time += 50*ms


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
#    takeSolisSoftwareFluorescenceImage(time+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, iXonImage = True, imagingDetuning = 0)

else : 
    andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
    camera = ch(andorCamera, 0)
    imageTime = time
    dtDeadMOT = 500*ms
    time = takeAbsorptionImage(imageTime, imageTime + dtDeadMOT, cropVector=imageCropVector, depumpAbsImage = depumpImage)
#    takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)

###############################################################
    

      
### Turn on MOT steady state ##################################################

MOT(time + 150*ms, leaveOn=True, cMOT = False)    # turn MOT back on


