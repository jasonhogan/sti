#setvar('probeLightAbsorptionImageOn', (probeAOMFreq, 25, 0) )

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

    tAomAbsorption = tAbsorption - dtAOMHoldoff
    tCameraAbsorption = tAbsorption - dtCameraHoldoff

    event(probeLightAOM, tAomAbsorption, probeLightOn )         # probe aom ON
    event(probeLightAOM, tAomAbsorption + dtProbeLight, probeLightOff )           # probe aom OFF
    meas(camera, tCameraAbsorption, (dtExposure,  'absorption image', filenameSuffix, cropVector))


    ## Reference image (absorption beam with no atoms) ##

    tAomReference = tReference - dtAOMHoldoff
    tCameraReference = tReference - dtCameraHoldoff

    event(probeLightAOM, tAomReference, probeLightOn )           # probe aom ON
    event(probeLightAOM, tAomReference + dtProbeLight, probeLightOff )             # probe aom OFF
    meas(camera, tCameraReference, (dtExposure, 'calibration image', filenameSuffix, cropVector))


    ## Background image (no absorption beam, no atoms) ##

    tCameraBackground = tReference + 100*ms

    meas(camera, tCameraBackground, (dtExposure, 'background image', filenameSuffix, cropVector))


    return (tCameraBackground + dtExposure);
