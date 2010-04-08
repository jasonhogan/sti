from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Tighter Focus again, f = 11.87: Dark spot absorption vs radius and probe frequency.  (~371 pm) 100 images per probe frequency for averaging at 55-60V at 0.25V per step''')

setvar('imageCropVector',(538,502,100))


setvar('dtDriftTime', 1.5*ms)
setvar('dtMOTLoad', 250*ms)
setvar('probeIntensity',30)
setvar('voltage1530', 0.87)
setvar('piezoVoltage', 60) ##54.6 is centered on resonance
setvar('diodeCurrent', 47.9)

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
fastAnalogOut6 = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 6)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
camera=dev('Andor iXon 885','ep-timing1.stanford.edu',0)
#wavemeter=dev('AndoAQ6140', 'eplittletable.stanford.edu',7)
#spectrumAnalyzer=dev('agilentE4411bSpectrumAnalyzer',  'eplittletable.stanford.edu', 18)
probeVortex=dev('Scanning Vortex', 'eplittletable.stanford.edu', 2)


motBlowAway = ch(digitalOut,2)
takeImage=ch(camera,0)
TA2 = ch(fastAnalogOut, 0)
TA3 = ch(fastAnalogOut, 1)
quadCoil = ch(slowAnalogOut, 2)
current1530 = ch(fastAnalogOut6,0)
aomSwitch0 = ch(dds, 0)
#wavelength1530=ch(wavemeter, 0)
#power1530 = ch(wavemeter, 1)
#absoptionLightFrequency = ch(spectrumAnalyzer, 0)
probeVortexCurrent = ch(probeVortex,0)
probeVortexPiezo = ch(probeVortex,1)



#AOM settings
aomFreq0 = 110

## TA Settings ##
voltageTA2 = 1.45
voltageTA3 = 1.5


def takeAbsorptionImage(tAbsorption, tReference):
    
    #tAbsorption = tTAOff + dtDriftTime
    #dtDeadMOT = 100*ms
    #tReference = tAbsorption + dtDeadMOT

    #### Probe AOM DDS settings
    probeDDS_Ch = ch(dds, 0)
    setvar('probeAOMFreq', 110)
    setvar('probeAOMpercent', 30)     #30 / 100 usually
    dtAOMHoldoff = 10*us

    #### Camera settings
    andorImageCh=ch(camera,0)
    filenameSuffix = 'absorption image'
    setvar('dtExposure', 100*us)
    dtCameraHoldoff = 5*us

    dtProbeLight = dtExposure / 2


    ## Absorbtion image (with atoms) ##

    tAomAbsorption = tAbsorption - dtAOMHoldoff
    tCameraAbsorption = tAbsorption - dtCameraHoldoff

    event(probeDDS_Ch, tAomAbsorption, (probeAOMFreq, probeAOMpercent, 0))         # probe aom ON
    event(probeDDS_Ch, tAomAbsorption + dtProbeLight, (probeAOMFreq, 0, 0))           # probe aom OFF
    meas(andorImageCh, tCameraAbsorption, (dtExposure,  'absorption image', filenameSuffix, imageCropVector))


    ## Reference image (absorption beam with no atoms) ##

    tAomReference = tReference - dtAOMHoldoff
    tCameraReference = tReference - dtCameraHoldoff

    event(probeDDS_Ch, tAomReference, (probeAOMFreq, probeAOMpercent, 0))           # probe aom ON
    event(probeDDS_Ch, tAomReference + dtProbeLight, (probeAOMFreq, 0, 0))             # probe aom OFF
    meas(andorImageCh, tCameraReference, (dtExposure, 'calibration image', filenameSuffix, imageCropVector))


    ## Background image (no absorption beam, no atoms) ##

    tCameraBackground = tReference + 100*ms

    meas(andorImageCh, tCameraBackground, (dtExposure, 'background image', filenameSuffix, imageCropVector))


    return (tCameraBackground + dtExposure);



# Define different blocks of the experiment
def MOT(tStart):

    ## TA Settings ##
    tTAOn = tStart + 100*ms
    tTAOff =  tTAOn + dtMOTLoad

    ## Quad Coil Settings ##
    quadCoilVoltage = 3.01

    ## Imaging Settings ##
    tAbsorptionImage = tTAOff + dtDriftTime
    tQuadCoilOff = tTAOff
    dtDeadMOT = 100*ms

    #################### events #######################

    event(TA2, tStart, 0)    # TA off MOT dark to kill any residual MOT
    event(TA3, tStart, 0)    # TA off
    event(current1530, tStart, voltage1530)    #1530 light on

    event(aomSwitch0,tStart, (aomFreq0, 0 ,0)) # AOM is off, so no imaging light
    event(motBlowAway, tStart, 0)                 #set cooling light to 10 MHz detuned via RF switch

#    meas(wavelength1530, tStart)
#    meas(power1530,tStart+500*ms)

#    meas(absoptionLightFrequency, tStart)

    ## Load the MOT ##  
    event(motBlowAway,  tTAOn, 0)                 #set cooling light to 10 MHz detuned via RF switch
    event(quadCoil, tTAOn, 0.6)  
    event(TA2, tTAOn, voltageTA2)                   # TA on
    event(TA3, tTAOn, voltageTA3)                   # TA on
    event(TA2, tTAOff, 0)    # TA off
    event(TA3, tTAOff, 0)    # TA off

    tDarkBackground = takeAbsorptionImage(tTAOff + dtDriftTime, tTAOff + dtDriftTime + dtDeadMOT)

      
    return tDarkBackground


# Global definitions

t0 = 11*us + 2*ms

time = t0
time = MOT(time)

tTAEndOfSequence = time +2*ms

event(TA2, tTAEndOfSequence, voltageTA2)
event(TA3, tTAEndOfSequence, voltageTA3)

#    event(aomSwitch0, tTAEndOfSequence, (aomFreq0, aomAmplitude0, 0)) #turn on absorbtion light 

#event(probeVortexCurrent, tTAEndOfSequence, diodeCurrent)
event(probeVortexPiezo, tTAEndOfSequence, piezoVoltage)
