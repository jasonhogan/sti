#needs channels.py and motFunction.py and depumpFunction.py

probeLightAOM = probeLightRFSwitch         ### renamed this crucial channel - now is an RF switch ### DJ 3/18/2011 

def takeSolisSoftwareAbsorptionImage(tAbsorption, expTime = 100*us, dtAbsorbtionLight = 50*us, iDus = False, depumpAbsImage = False) : 
    
    if (Fis1Imaging) :
        setvar('dtProbeLightBuffer',15*us) #ensures camera is on before the probe light and shuts off afterwards
        setvar('dtProbeLight', dtAbsorbtionLight)
        setvar('dtExposure', dtProbeLight+2*dtProbeLightBuffer)
    else :
        setvar('dtProbeLightBuffer',15*us) #ensures camera is on before the probe light and shuts off afterwards
        setvar('dtProbeLight', dtAbsorbtionLight)    #20*25*us
        setvar('dtExposure', dtProbeLight+2*dtProbeLightBuffer)

    dtCameraDelay = -5*us + dtProbeLightBuffer

    tShutterOpen = tAbsorption - dtShutterBuffer
    tShutterClose = tAbsorption + expTime + dtShutterBuffer
    tAOM  = tAbsorption - 0*dtAOMHoldoff
    tCameraTrigger = tAbsorption - dtCameraDelay

    event(probeLightShutter, tShutterOpen, 1)                                                 #open probe light shutter
    event(probeLightShutter, tShutterClose, 0)                                                 #close probe light shutter
    
    event(probeLightRFSwitch, tAOM, probeLightOn)                                            #turn on absorbtion light
    event(probeLightRFSwitch, tAOM + dtProbeLight, probeLightOff)              #turn off absorbtion light

    event(digitalSynch, tCameraTrigger - 10*ms,0) #trigger webscope for quad coil ramp
    event(digitalSynch, tCameraTrigger,1) 
    event(digitalSynch, tCameraTrigger + 10*ms,0)

    if (depumpAbsImage):
        depumpMOT(tAbsorption - 1.1*us, pumpingTime = dtProbeLight+2.2*us)

    event(cameraTriggerSlow, tCameraTrigger, 5)
    event(cameraTriggerSlow, tCameraTrigger + expTime, 0)

    if (iDus):
        ### turn on MOT light ###
        tFluorescence = tCameraTrigger + expTime + 100*us
        imagingDetuning=0
        event(ch(dds,1), tFluorescence - 20*us, (129 + imagingDetuning, 100, 0) )

        dtFluorescenceLight = 20*100*us

        turnMOTLightOn(tFluorescence)
        turnMOTLightOff(tFluorescence + dtFluorescenceLight)

        event(iDusCameraTrigger, tFluorescence + 10*us, 5)
        event(iDusCameraTrigger, tFluorescence + 10*us + dtFluorescenceLight, 0)

    return (tCameraTrigger + expTime);


def takeAbsorptionImage(tAbsorption, tReference, cropVector = (500,500,499), depumpAbsImage = False):
   
    #### Camera settings

    filenameSuffix = 'absorption image'
    if (Fis1Imaging) :
        setvar('dtProbeLightBuffer',150*us) #ensures camera is on before the probe light and shuts off afterwards
        setvar('dtProbeLight', 35*us)    #20*25*us
        setvar('dtExposure', dtProbeLight+2*dtProbeLightBuffer)
    else :
        setvar('dtProbeLightBuffer',150*us) #ensures camera is on before the probe light and shuts off afterwards
        setvar('dtProbeLight', 35*us)    #20*25*us
        setvar('dtExposure', dtProbeLight+2*dtProbeLightBuffer)

#        setvar('dtExposure', 50*us)
#        setvar('dtProbeLight', 20*us)
        
    dtCameraHoldoff = -5*us + dtProbeLightBuffer                 #5*us before 07/15/11; 15*us gives a 10*us buffer between camera triggering and light turning on
    

    ## Absorbtion image (with atoms) ##

    tShutterOpenAbsorption = tAbsorption - dtShutterBuffer
    tShutterCloseAbsorption = tAbsorption + dtExposure + dtShutterBuffer
    tAomAbsorption = tAbsorption - 0*dtAOMHoldoff
    tCameraAbsorption = tAbsorption - dtCameraHoldoff

    event(probeLightShutter, tShutterOpenAbsorption, 1)                                             #open probe light shutter
    event(probeLightShutter, tShutterCloseAbsorption, 0)                                             #close probe light shutter
    
    event(probeLightRFSwitch, tAomAbsorption, probeLightOn )                                        # probe aom ON
    event(probeLightRFSwitch, tAomAbsorption + dtProbeLight, probeLightOff )                 # probe aom OFF

#    event(digitalSynch, tAbsorption - 10*ms,0) #trigger webscope for quad coil ramp
#    event(digitalSynch, tAbsorption,1) 
#    event(digitalSynch, tAbsorption + 10*ms,0)

    if (depumpAbsImage) :
        depumpMOT(tAbsorption - 1.1*us, pumpingTime = dtProbeLight + 2.2*us)
    
    meas(camera, tCameraAbsorption, (dtExposure,  'absorption image', filenameSuffix, cropVector))

    ## Reference image (absorption beam with no atoms) ##

    tShutterOpenReference= tReference - dtShutterBuffer
    tShutterCloseReference = tReference + dtExposure + dtShutterBuffer

    tAomReference = tReference - 0*dtAOMHoldoff
    tCameraReference = tReference - dtCameraHoldoff

    event(probeLightShutter, tShutterOpenReference, 1)                                                 #open probe light shutter
    event(probeLightShutter, tShutterCloseReference, 0)                                                 #close probe light shutter

    event(probeLightRFSwitch, tAomReference, probeLightOn )                                            # probe aom ON
    event(probeLightRFSwitch, tAomReference + dtProbeLight, probeLightOff )                     # probe aom OFF

    if (depumpAbsImage) :
        depumpMOT(tReference - 1.1*us, pumpingTime = dtProbeLight + 2.2*us)

    meas(camera, tCameraReference, (dtExposure, 'calibration image', filenameSuffix, cropVector))

    ## Background image (no absorption beam, no atoms) ##

    tCameraBackground = tReference + 100*ms
    meas(camera, tCameraBackground, (dtExposure, 'background image', filenameSuffix, cropVector))

    return (tCameraBackground + dtExposure);


def takeSolisSoftwareFluorescenceImage(tFluorescence, dtFluorescenceExposure = 10*ms, leaveMOTLightOn = False, iDusImage = False, iXonImage = False, imagingDetuning = 10) : 

    dtCameraDelay = 5*us

    dtTAsOn = 0.01*ms
    dtTAsOff = 0.01*ms

    event(ch(dds,1), tFluorescence - 20*us, (129 + imagingDetuning, 100, 0) )
    event(repumpVariableAttenuator, tFluorescence, 10)

    tTAsOn = tFluorescence + dtTAsOn
    tTAsOff = tTAsOn + dtFluorescenceExposure  - dtTAsOff - dtTAsOn

    tCameraTrigger = tFluorescence - dtCameraDelay

    turnMOTLightOn(tTAsOn)
    turnMOTLightOff(tTAsOff)

    if(leaveMOTLightOn):
        turnMOTLightOn(tCameraTrigger + dtFluorescenceExposure + dtTAsOn)

    if(iDusImage) :
        event(iDusCameraTrigger, tCameraTrigger + 10*us, 5)
        event(iDusCameraTrigger, tCameraTrigger + 10*us + dtFluorescenceExposure, 0)

    if(iXonImage) :
        event(cameraTriggerSlow, tCameraTrigger,5)
        event(cameraTriggerSlow, tCameraTrigger + dtFluorescenceExposure + 10*ms, 0)
    

    return (tCameraTrigger + dtFluorescenceExposure);


def takeFluorescenceImage(tFluorescence, dtFluorescenceExposure=10*ms, leaveMOTLightOn=False, cropVector = (500,500,499), indexImages=False, imageIndex=0, repumpOn = True):
    
    #### Camera settings

    if(indexImages):
        filenameSuffix = 'fluorescence image_'+str(imageIndex)
    else:
        filenameSuffix = 'fluorescence image'

    dtCameraHoldoff = 5*us

    dtTAsOn = 10*us
    dtFluorescenceLightOn = dtFluorescenceExposure - 20*us

    ## Fluorescence image ##

    # If the MOT light isn't already on, turn TAs on after the picture starts (to avoid streaking)
    tTAsOn = tFluorescence + dtTAsOn
    tTAsOff = tTAsOn + dtFluorescenceLightOn
    tCameraFluorescence = tFluorescence - dtCameraHoldoff

    setvar('deltaFimage',10.5)

    event(ch(dds,1), tFluorescence, (ddsMotFrequency-deltaFimage, 100, 0) )    #ddsMotFrequency-7.5+3

    if(repumpOn) :
        event(repumpVariableAttenuator, tFluorescence, 10)    ## Repump on
    else :
        event(repumpVariableAttenuator, tFluorescence, 0)    ## Repump off

    turnMOTLightOn(tTAsOn)
    turnMOTLightOff(tTAsOff)

    meas(camera, tCameraFluorescence, (dtFluorescenceExposure,  'fluorescence image', filenameSuffix, cropVector))

#    if(leaveMOTLightOn):
#        turnMOTLightOn(tCameraFluorescence + dtFluorescenceExposure + dtTAsOn)

    return (tCameraFluorescence + dtFluorescenceExposure + dtTAsOn);
