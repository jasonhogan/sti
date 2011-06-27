#needs channels.py and motFunction.py

probeLightAOM = probeLightRFSwitch         ### renamed this crucial channel - now is an RF switch ### DJ 3/18/2011 

def takeSolisSoftwareAbsorptionImage(tAbsorption, expTime = 100*us, dtAbsorbtionLight = 50*us, iDus = False) : 
    
    dtCameraDelay = 5*us

    tShutterOpen = tAbsorption - dtShutterBuffer
    tShutterClose = tAbsorption + expTime + dtShutterBuffer
    tAOM  = tAbsorption - dtAOMHoldoff
    tCameraTrigger = tAbsorption - dtCameraDelay

    event(probeLightShutter, tShutterOpen, 1)                                                 #open probe light shutter
    event(probeLightShutter, tShutterClose, 0)                                                 #close probe light shutter
    
    event(probeLightRFSwitch, tAOM, probeLightOn)                                            #turn on absorbtion light
    event(probeLightRFSwitch, tAOM + dtAbsorbtionLight, probeLightOff)              #turn off absorbtion light

    event(cameraTriggerSlow, tCameraTrigger, 5)
    event(cameraTriggerSlow, tCameraTrigger + expTime, 0)

    if (iDus):
        ### turn on MOT light ###
        tFluorescence = tCameraTrigger + expTime + 100*us
        
        dtFluorescenceLight = 1*ms

        turnMOTLightOn(tFluorescence)
        turnMOTLightOff(tFluorescence + dtFluorescenceLight)

        event(iDusCameraTrigger, tFluorescence + 10*us, 5)
        event(iDusCameraTrigger, tFluorescence + 10*us + dtFluorescenceLight, 0)

    return (tCameraTrigger + expTime);


def takeAbsorptionImage(tAbsorption, tReference, cropVector = (500,500,499)):
   
    #### Camera settings

    filenameSuffix = 'absorption image'
    setvar('dtExposure', 50*us)

    dtCameraHoldoff = 5*us

    setvar('dtProbeLight', 25*us)

    ## Absorbtion image (with atoms) ##

    tShutterOpenAbsorption = tAbsorption - dtShutterBuffer
    tShutterCloseAbsorption = tAbsorption + dtExposure + dtShutterBuffer
    tAomAbsorption = tAbsorption - dtAOMHoldoff
    tCameraAbsorption = tAbsorption - dtCameraHoldoff

    event(probeLightShutter, tShutterOpenAbsorption, 1)                                             #open probe light shutter
    event(probeLightShutter, tShutterCloseAbsorption, 0)                                             #close probe light shutter
    
    event(probeLightRFSwitch, tAomAbsorption, probeLightOn )                                        # probe aom ON
    event(probeLightRFSwitch, tAomAbsorption + dtProbeLight, probeLightOff )                 # probe aom OFF
    meas(camera, tCameraAbsorption, (dtExposure,  'absorption image', filenameSuffix, cropVector))

    ## Reference image (absorption beam with no atoms) ##

    tShutterOpenReference= tReference - dtShutterBuffer
    tShutterCloseReference = tReference + dtExposure + dtShutterBuffer

    tAomReference = tReference - dtAOMHoldoff
    tCameraReference = tReference - dtCameraHoldoff

    event(probeLightShutter, tShutterOpenReference, 1)                                                 #open probe light shutter
    event(probeLightShutter, tShutterCloseReference, 0)                                                 #close probe light shutter

    event(probeLightRFSwitch, tAomReference, probeLightOn )                                            # probe aom ON
    event(probeLightRFSwitch, tAomReference + dtProbeLight, probeLightOff )                     # probe aom OFF
    meas(camera, tCameraReference, (dtExposure, 'calibration image', filenameSuffix, cropVector))

    ## Background image (no absorption beam, no atoms) ##

    tCameraBackground = tReference + 100*ms
    meas(camera, tCameraBackground, (dtExposure, 'background image', filenameSuffix, cropVector))

    return (tCameraBackground + dtExposure);


def takeSolisSoftwareFluorescenceImage(tFluorescence, dtFluorescenceExposure = 10*ms, leaveMOTLightOn = False, iDusImage = False, imagingDetuning = 10) : 

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

    else:
        event(cameraTriggerSlow, tCameraTrigger, 5)
        event(cameraTriggerSlow, tCameraTrigger + dtFluorescenceExposure, 0)
    

    return (tCameraTrigger + dtFluorescenceExposure);


def takeFluorescenceImage(tFluorescence, dtFluorescenceExposure=10*ms, leaveMOTLightOn=False, cropVector = (500,500,499), indexImages=False, imageIndex=0):
    
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

#    turnMOTLightOn(tTAsOn)
#    turnMOTLightOff(tTAsOff)

    meas(camera, tCameraFluorescence, (dtFluorescenceExposure,  'fluorescence image', filenameSuffix, cropVector))

#    if(leaveMOTLightOn):
#        turnMOTLightOn(tCameraFluorescence + dtFluorescenceExposure + dtTAsOn)

    return (tCameraFluorescence + dtFluorescenceExposure + dtTAsOn);
