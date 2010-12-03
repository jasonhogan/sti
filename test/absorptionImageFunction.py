#setvar('probeLightAbsorptionImageOn', (probeAOMFreq, 25, 0) )

def takeSolisSoftwareAbsorptionImage(tAbsorption) : 
    dtAbsorbtionLight = 50*us
    dtCameraDelay = 5*us
    expTime = 100*us

    tShutterOpen = tAbsorption - dtShutterHoldOff
    tShutterClose = tAbsorption + dtShutterHoldOff
    tAOM  = tAbsorption - dtAOMHoldoff
    tCameraTrigger = tAbsorption - dtCameraDelay

    event(probeLightShutter, tShutterOpen, 1)         #open probe light shutter
    event(probeLightShutter, tShutterClose, 0)         #close probe light shutter
    
    event(probeLightAOM, tAOM, probeLightOn)               #turn on absorbtion light
    event(probeLightAOM, tAOM + dtAbsorbtionLight, probeLightOff)              #turn off absorbtion light

    event(cameraTriggerSlow, tCameraTrigger, 5)
    event(cameraTriggerSlow, tCameraTrigger + expTime, 0)
    
#    event(cameraTrigger, tCameraTrigger, 1)
#    event(cameraTrigger, tCameraTrigger + expTime, 0)

    return (tCameraTrigger + expTime);

def takeAbsorptionImage(tAbsorption, tReference, cropVector = (500,500,499)):
    
    #tAbsorption = tTAOff + dtDriftTime
    #dtDeadMOT = 100*ms
    #tReference = tAbsorption + dtDeadMOT


    #### Camera settings

    filenameSuffix = 'absorption image'
    setvar('dtExposure', 100*us)
    dtCameraHoldoff = 5*us

    dtProbeLight = dtExposure / 2

    ## Set Light used for frequency lock to 10 MHz Red Detuned (normal cooling)
#    event(motFrequencySwitch, tAbsorption - rfSwitchHoldOff - lockAcquisitionTime, 0)

    ## Absorbtion image (with atoms) ##

    tShutterOpenAbsorption = tAbsorption - dtShutterHoldOff
    tShutterCloseAbsorption = tAbsorption + dtShutterHoldOff
    tAomAbsorption = tAbsorption - dtAOMHoldoff
    tCameraAbsorption = tAbsorption - dtCameraHoldoff

    event(probeLightShutter, tShutterOpenAbsorption, 1)         #open probe light shutter
    event(probeLightShutter, tShutterCloseAbsorption, 0)         #close probe light shutter
    
    event(probeLightAOM, tAomAbsorption, probeLightOn )         # probe aom ON
    event(probeLightAOM, tAomAbsorption + dtProbeLight, probeLightOff )           # probe aom OFF
    meas(camera, tCameraAbsorption, (dtExposure,  'absorption image', filenameSuffix, cropVector))


    ## Reference image (absorption beam with no atoms) ##

    tShutterOpenReference= tReference - dtShutterHoldOff
    tShutterCloseReference = tReference + dtShutterHoldOff

    tAomReference = tReference - dtAOMHoldoff
    tCameraReference = tReference - dtCameraHoldoff

    event(probeLightShutter, tShutterOpenReference, 1)         #open probe light shutter
    event(probeLightShutter, tShutterCloseReference, 0)         #close probe light shutter

    event(probeLightAOM, tAomReference, probeLightOn )           # probe aom ON
    event(probeLightAOM, tAomReference + dtProbeLight, probeLightOff )             # probe aom OFF
    meas(camera, tCameraReference, (dtExposure, 'calibration image', filenameSuffix, cropVector))


    ## Background image (no absorption beam, no atoms) ##

    tCameraBackground = tReference + 100*ms

    meas(camera, tCameraBackground, (dtExposure, 'background image', filenameSuffix, cropVector))


    return (tCameraBackground + dtExposure);
