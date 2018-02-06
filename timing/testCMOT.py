from stipy import *
from math import fabs
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0
urad = 0.000001

include('channels.py')
include('experimentalParameters.py')
include('makeCMOTFunction.py')
include('repumpFunction.py')
include('depumpFunction.py')
include('andorCameraFunctions.py')
include('raman.py')
include('biasCoils.py')

#setvar('dtDriftTimeSequence', 1000*us)
#setvar('dtDriftTime', dtDriftTimeSequence)
setvar('dtDriftTime', 5000*us)
#setvar('deltaImagingFreqMHz', -18.0)

setvar('desc', "CMOT test; MOT load time " + str(MOTLoadTime/s) + " s; DriftTime " + str(dtDriftTime/ms) + " ms; Det " + str(deltaImagingFreqMHz) +" MHz.")

##setvar('imageCropVector',(500, 500, 490))
setvar('imageCropVector',(1, 60+30, 1003-50, 913-50))

setvar ('opticallyRepump', True) #True
setvar('opticallyDepump', False)

setvar('realTime', False)
setvar('depumpImage', False)
setvar('absorptionImage', True)

#event(ch(triggerTS2,8), 2*ns, "Wait")
#event(ch(trigger,8), 5*ns, "Wait All")
#tExternalTriggerTS2 = 100*ms + 500*ms
#
#event(TS2externalTrigger, tExternalTriggerTS2, 1)
#event(TS2externalTrigger, tExternalTriggerTS2 + 10*ms, 0)

######## Prepare atom cloud ########

tStart = 100*us+5*ms + 1*ms#+tExternalTriggerTS2
settag("Here")
#event(ch(trigger,8), 2*ns, "Wait All")
#event(ch(trigger,8), tStart, "Wait")

#event(imagingOffsetFrequency, tStart, imagingResonanceFreq + deltaImagingFreqMHz*1e6)

#meas(masterLockCheck, tStart, "Master lock check at start of sequence.")
#meas(imagingLockCheck, tStart, "Imaging lock check at start of sequence.")
setvar('zBias', -.0) #0.5 #2.8
setvar('zGrad', 0) #0.5

tBiasCoilsOn = tStart - 1.05*ms

event(zBiasTopRelay, tStart - 10*us, 6)#6
setZBiasTop(tBiasCoilsOn,  zLeft + zGrad - zBias)
setZBiasBot(tBiasCoilsOn +10*us, zRight + zGrad + zBias)

setF1BlowawaySwitch(tStart - 0.2*ms, 'Repump')
#event(cooling85Shutter, tStart - 0.1*ms-10*us, 0)
event(TARepump, tStart - 50*us, voltageTARepump)
event(TA87, tStart - 55*us, voltageTA87)
event(TA85, tStart - 60*us, voltageTA85)

##########################
######## Safety ##########
##########################

event(topSafetySwitch, tStart -1*ms + 10*us, 0)
event(topVCA, tStart -1*ms + 10*us, 0)
#event(quadrupoleShortSwitch, tStart - 2*ms + 2*us, 0) 

#event(TA1, tStart - 2*ms, voltageTA1)
event(imagingDetuning, tStart, 80 + deltaImagingFreqMHz)
time = makeCMOT(tStart, Rb87 = coolRb87, Rb85 = coolRb85)
#time = MOT(tStart, rb87 = coolRb87, rb85 = coolRb85,  cMOT = False, dtMOTLoad=MOTLoadTime)

#time += 10*ms

#timeFluorescenceExposure = time-.05*s

event(repumpSwitch, time - 5*us, 0)                               
event(motFrequencySwitch, time - 10*us, 1)        
#time = turnMOTLightOff(time)
turnRFModulationOff(time)
time = turn3DXYLightOff(time)

            
#### Alternate: turn 2D light off first
#time = turn2DMOTLightOff(time - 2*ms)
#
#turn3DZLightOff(time)
#turn3DXYLightOff(time)
#turnRFModulationOff(time)
##### End Alternate


#event(TA8, time-2*ms, 0)

#
#time += 10.4*ms
#pulse = 100*us
##event(TA7, time-0.05*ms, ta7MotVoltage)                             # z-Axis seed TA on
#event(zAxisRfSwitch, time, 1)
#event(TAZ, time, ta4MotVoltage)
#time +=pulse
##event(TA7, time, ta7OffVoltage)                             # z-Axis seed TA on
#event(TAZ, time, ta4OffVoltage)     
#time += 10*us



time += 0.8*ms




######## Measure the result: Snap off field, repump, and image ########

### Snap off magnetic field
#time = rampDownQuadCoils(time, rapidOff = True)
snapOffField(time)

time += 700*us               #allow fields to shut off; leave time for repump pulse


### Image

time += dtDriftTime


setvar('depumpTime', 70*us)  #70*us

if (opticallyDepump) :
    time = depumpMOT(time + 10*us, pumpingTime = depumpTime)    #10ms
    time += 10*us


setvar('repumpTime', 200*us)  #200*us  #1000*us #25*us with new cooling shutter


### repump out of F = 1'
if (opticallyRepump) :
    time = repumpMOT(time + 10*us, pumpingTime = repumpTime, ta4RepumpVoltage=1.00*TA4RepumpVoltage)
    time += 10*us #+10*us
else :
    event(cooling87Shutter, time - 7*ms, 0)  #7*ms instead of 5*ms so that cooling 87 can turn back on sooner at the end of this function.
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
#        time+=1*ms
#        repumpMOT(imageTime - 300*us - 2*us, pumpingTime = 10*ms, ta4RepumpVoltage=1.00*TA4RepumpVoltage)
        time = takeAbsorptionImage(imageTime, imageTime + dtDeadMOT, cropVector=imageCropVector, depumpAbsImage = depumpImage, imageSmallFraction = False)
#        takeSolisSoftwareFluorescenceImage(imageTime+100*us, dtFluorescenceExposure = 1*ms, leaveMOTLightOn = False, iDusImage = True, imagingDetuning = 0)
#        takeFluorescenceExposure(timeFluorescenceExposure, dtFluorescenceExposure=1*ms, cropVector = imageCropVector)
    else:
       time = takeFluorescenceImage(imageTime, dtFluorescenceExposure = 10*ms, cropVector = imageCropVector, repumpOn = True)



###############################################################
    

      
### Turn on MOT steady state ##################################################

tSteadStateMot = time + 150*ms 

MOT(tSteadStateMot, dtMOTLoad = 11*ms, rb87 = coolRb87, rb85 = coolRb85, leaveOn=True, cMOT = False, motQuadCoilCurrent = MOTcurrent)    # turn MOT back on
#event(digitalSynch, time+1*s,0) 

event(digitalSynch, triggerTime - 10*ms,0) #trigger webscope for quad coil ramp
event(digitalSynch, triggerTime,1) 
event(digitalSynch, triggerTime + 10*ms,0)

#meas(masterLockCheck, tSteadStateMot, "Master lock check at end of sequence.")
#meas(imagingLockCheck, tSteadStateMot, "Imaging lock check at end of sequence.")

