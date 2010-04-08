from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program


setvar('imageCropVector',(500,500,480))

setvar('dtMOTLoad', 500*ms)
setvar('cmotTime', 20*ms)
setvar('cmotFieldMultiplier',1)
setvar('probeIntensity',30)

digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
fastAnalogOut6 = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 6)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
camera=dev('Andor iXon 885','ep-timing1.stanford.edu',0)
wavemeter=dev('AndoAQ6140', 'eplittletable.stanford.edu',7)
spectrumAnalyzer=dev('agilentE4411bSpectrumAnalyzer',  'eplittletable.stanford.edu', 18)



motFrequencySwitch = ch(digitalOut,2)
repumpFrequencySwitch = ch(digitalOut,5)
takeImage=ch(camera,0)
TA2 = ch(fastAnalogOut, 0)
TA3 = ch(fastAnalogOut, 1)
quadCoil = ch(slowAnalogOut, 2)
current1530 = ch(fastAnalogOut6,0)
aomSwitch0 = ch(dds, 0)
starkShiftingAOM = ch(dds,2)

absoptionLightFrequency = ch(spectrumAnalyzer, 0)


#RF Switch Settings
rfSwitchHoldOff = 10*us
lockAcquisitionTime = 1*ms

#Stark Shifting Beam Settings
setvar('starkShiftOn', (75, 90, 0) )
setvar('starkShiftOff', (75, 0, 0) ) 

#Imaging AOM settings
aomFreq0 = 110



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

    ## Set Light used for frequency lock to 10 MHz Red Detuned (normal cooling)
    event(motFrequencySwitch, tAbsorption - rfSwitchHoldOff - lockAcquisitionTime, 0)

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
def MOT(tStart, leaveOn=False, tClearTime=100*ms, cMOT=True):

    ## TA Settings ##
    tTAOn = tStart+tClearTime
    tTAOff =  tTAOn + dtMOTLoad
    voltageTA2 = 1.45
    voltageTA3 = 1.5

    ## Quad Coil Settings ##
    quadCoilCurrent = 0.6
    cmotQuadCoilCurrent = cmotFieldMultiplier * quadCoilCurrent
    quadCoilHoldOff = 10*ms

    ## Imaging Settings ##
    tAbsorptionImage = tTAOff + dtDriftTime
    tQuadCoilOff = tTAOff

    #################### events #######################

    tResetMOT = tStart

    ## Clear away any atoms  
    event(TA2, tResetMOT, 0)    # TA off MOT dark to kill any residual MOT
    event(TA3, tResetMOT, 0)    # TA off

    event(motFrequencySwitch,  tResetMOT, 0)                       # set cooling light to 10 MHz detuned via RF switch
    event(repumpFrequencySwitch,  tResetMOT, 0)                 # set repump light on resonance via RF switch

    ## Load the MOT ##  
    event(quadCoil, tTAOn, quadCoilCurrent)  
    event(TA2, tTAOn, voltageTA2)                   # TA on
    event(TA3, tTAOn, voltageTA3)                   # TA on

    if(leaveOn) :
        return tTAOn

    if(cMOT) :
        ## switch to a CMOT ##
        event(motFrequencySwitch, tTAOff - rfSwitchHoldOff - cmotTime, 1)                  #set cooling light to 90 MHz detuned via RF switch
        event(repumpFrequencySwitch, tTAOff - rfSwitchHoldOff - cmotTime, 1)            #set repump light to -17dBm via RF switch
        event(quadCoil, tTAOff - quadCoilHoldOff - cmotTime, cmotQuadCoilCurrent)

    ## finish loading the MOT
    event(TA2, tTAOff, 0)             # TA off
    event(TA3, tTAOff, 0)             # TA off
    event(quadCoil, tTAOff, 0)      # turn off the quad coils

    return tTAOff


###########################################################################
###########################################################################


setvar('dtDriftTime', 1*ms)


# Global definitions

t0 = 11*us + 2*ms

meas(ch(wavemeter, 0), t0, "Measure 1530 frequency")
meas(absoptionLightFrequency, t0)
event(starkShiftingAOM, 100*us, starkShiftOff)
event(aomSwitch0, t0, (aomFreq0, 0 ,0))             # AOM is off, so no imaging light

time = t0
time = MOT(time, tClearTime=100*ms, cMOT=False)
motFinishedLoading=time


##1530 experiment

setvar('darkSpotOn',True)
setvar('depumpMOT',True)


if(darkSpotOn) :
    setvar('desc',"Atoms in dark spot--1530 On")
    event(starkShiftingAOM, t0+50*ms, starkShiftOn)
    event(starkShiftingAOM, motFinishedLoading, starkShiftOff)
else :
    setvar('desc',"Atoms in dark spot--1530 Off")
    event(starkShiftingAOM, t0+50*ms, starkShiftOff)
    event(starkShiftingAOM, motFinishedLoading, starkShiftOff)


#depump the MOT
depumpTime = 5*ms

if(depumpMOT) :
    event(repumpFrequencySwitch, motFinishedLoading - depumpTime, 1)  
    event(motFrequencySwitch, motFinishedLoading - depumpTime, 1)               

##Image

dtDeadMOT = 100*ms
time = takeAbsorptionImage(time + dtDriftTime, time + dtDriftTime + dtDeadMOT)


## Turn on MOT stead state

tTAEndOfSequence = time +2*ms
time = MOT(tTAEndOfSequence, leaveOn=True)    # turn MOT back on

