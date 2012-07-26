from math import fabs

ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

### Global RF Switch Holdoffs ###
setvar('rfSwitchHoldOff', 10*us)
setvar('lockAcquisitionTime', 1*ms)

### Cooling & Rempump VCOs ###
setvar('ddsMotFrequency',139)

### Probe light shutter ###
dtShutterBuffer = 5*ms  # uncertainty in shutter timing

### Probe light RF switch settings ###
setvar('probeLightOn', 1 )
setvar('probeLightOff', 0 )
setvar('dtAOMHoldoff', 10*us)

### Imaging ###

setvar('deltaImagingFreqMHz', 0)

#setvar('deltaImagingFreqMHzSeq', 1.3)
#setvar('deltaImagingFreqMHz', deltaImagingFreqMHzSeq)

setvar('ImagingTransition', "2->3")
#setvar('ImagingTransition', "1->2")
#setvar('ImagingTransition', "1->1")


if(ImagingTransition == "1->1" or ImagingTransition == "1->2") :
    setvar('Fis1Imaging', True)
else :
    setvar('Fis1Imaging', False)

if(ImagingTransition == "2->3") :
    setvar('imagingResonanceFreq', 1.802e9 - 2.18e6)
if(ImagingTransition == "1->2") :
    setvar('imagingResonanceFreq', 4.766e9)
if(ImagingTransition == "1->1") :
    setvar('imagingResonanceFreq', 4.610e9)


### Tapered Amplifiers ###
setvar('voltageTA1', 1.1)                   # this is only intended to be switched on/off for the evening #1.755
setvar('voltageTA2', 1.95)     #1.9            # switch off during every mag trap cycle
setvar('voltageTA3', 1.7)                   # switch off during every mag trap cycle & ramp during the cMOT
setvar('ta4MotVoltage', 0.50) #0.42 #0.4           # controls a fancy power supply - this voltage setting for MOT, TA5 & TA6 are set to 0 during a MOT
setvar('ta7MotVoltage', .85)             # this is only intended to be switched on/off for the evening

setvar('ta4OffVoltage', 0.1) #TA4 slow turn on below ~0.8V, beware ASE? AFS 061511

### RF Knife ###
#setvar('ddsRbResonanceFreq', 62.42606)
setvar('ddsRbResonanceFreq', 500)

### Atom Interferometer Lasers ###
setvar('zAxisAomMot', (100, 100, 0)) #setvar('zAxisAomMot', (200, 100, 0))
setvar('zAxisAomOff', (200, 0, 0))

setvar('braggAOM1MOT', (100, 100, 0))
setvar('braggAOM1Off', (100, 0, 0))

### Magnetic fields ###
setvar('varFullMagneticTrapCurrent', 250) #285
#setvar('varDischargeCurrent', 35)    #35

setvar('intermediateTrapCurrent', 150)

setvar('imagingCurrent', 58)
setvar('magTrapTransferCurrent', 58)
