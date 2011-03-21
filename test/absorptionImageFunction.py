probeLightAOM = probeLightRFSwitch

def takeSolisSoftwareAbsorptionImage(tAbsorption, expTime = 100*us) : 
    dtAbsorbtionLight = 50*us
    dtCameraDelay = 5*us

    tShutterOpen = tAbsorption - dtShutterHoldOff
    tShutterClose = tAbsorption + dtShutterHoldOff
    tAOM  = tAbsorption - dtAOMHoldoff
    tCameraTrigger = tAbsorption - dtCameraDelay

    event(probeLightShutter, tShutterOpen, 1)         #open probe light shutter
    event(probeLightShutter, tShutterClose, 0)         #close probe light shutter
    
    event(probeLightRFSwitch, tAOM, probeLightOn)               #turn on absorbtion light
    event(probeLightRFSwitch, tAOM + dtAbsorbtionLight, probeLightOff)              #turn off absorbtion light

    event(cameraTriggerSlow, tCameraTrigger, 5)
    event(cameraTriggerSlow, tCameraTrigger + expTime, 0)

    return (tCameraTrigger + expTime);

def takeAbsorptionImage(tAbsorption, tReference, cropVector = (500,500,499)):

    #### Camera settings
    filenameSuffix = 'absorption image'
    setvar('dtExposure', 100*us)
    dtCameraHoldoff = 5*us

    dtProbeLight = dtExposure - 20*us

    ## Absorbtion image (with atoms) ##

    tShutterOpenAbsorption = tAbsorption - dtShutterHoldOff
    tShutterCloseAbsorption = tAbsorption + dtShutterHoldOff
    tAomAbsorption = tAbsorption - dtAOMHoldoff
    tCameraAbsorption = tAbsorption - dtCameraHoldoff

    event(probeLightShutter, tShutterOpenAbsorption, 1)         #open probe light shutter
    event(probeLightShutter, tShutterCloseAbsorption, 0)         #close probe light shutter

    event(probeLightRFSwitch, tAomAbsorption, probeLightOn)               #turn on absorbtion light
    event(probeLightRFSwitch, tAomAbsorption + dtProbeLight, probeLightOff)              #turn off absorbtion light
    meas(camera, tCameraAbsorption, (dtExposure,  'absorption image', filenameSuffix, cropVector))


    ## Reference image (absorption beam with no atoms) ##

    tShutterOpenReference= tReference - dtShutterHoldOff
    tShutterCloseReference = tReference + dtShutterHoldOff

    tAomReference = tReference - dtAOMHoldoff
    tCameraReference = tReference - dtCameraHoldoff

    event(probeLightShutter, tShutterOpenReference, 1)         #open probe light shutter
    event(probeLightShutter, tShutterCloseReference, 0)         #close probe light shutter

    event(probeLightRFSwitch, tAomReference, probeLightOn )           # probe aom ON
    event(probeLightRFSwitch, tAomReference + dtProbeLight, probeLightOff )             # probe aom OFF
    meas(camera, tCameraReference, (dtExposure, 'calibration image', filenameSuffix, cropVector))


    ## Background image (no absorption beam, no atoms) ##

    tCameraBackground = tReference + 100*ms

    meas(camera, tCameraBackground, (dtExposure, 'background image', filenameSuffix, cropVector))


    return (tCameraBackground + dtExposure);
