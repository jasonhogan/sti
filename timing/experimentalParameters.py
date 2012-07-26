from math import fabs

ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

### Global RF Switch Holdoffs ###
setvar('rfSwitchHoldOff', 10*us)
setvar('lockAcquisitionTime', 1*ms)

### Cooling & Rempump VCOs ###
setvar('ddsMotFrequency',139) #139

### Probe light shutter ###
dtShutterBuffer = 5*ms  # uncertainty in shutter timing

### Probe light RF switch settings ###
setvar('probeLightOn', 1 )
setvar('probeLightOff', 0 )
setvar('dtAOMHoldoff', 10*us)

### Imaging ###

setvar('deltaImagingFreqMHz', -18)  #negative detunings are easier for the lock

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
    setvar('imagingResonanceFreq', 1.802e9 - 2.18e6 - 1.51e6 - 27e6)
if(ImagingTransition == "1->2") :
    setvar('imagingResonanceFreq', 4.766e9)
if(ImagingTransition == "1->1") :
    setvar('imagingResonanceFreq', 4.610e9)


### Tapered Amplifiers ###
setvar('voltageTA1', 1.26)   #1.26 with intensity feedback
setvar('voltageTA2', 8.77)   #8.71 (4.04 with intensity feedback, not used now)
setvar('voltageTA3', 1.3)     #1.22   # switch off during every mag trap cycle & ramp during the cMOT
#setvar('ta4MotVoltageSeq', 0.42)
setvar('ta4MotVoltage', 0.35+0.05) #0.42 #0.285 #0.304 #0.42  # controls a fancy power supply - this voltage setting for MOT, TA5 & TA6 are set to 0 during a MOT
setvar('ta4DepumpVoltage', 0.50)
setvar('ta7MotVoltage', 0.317)   #0.44 with intensity feedback, MOT light only #0.20
setvar('voltageTA8', 1.4)   # 1.28 with intensity feedback #0.99

setvar('ta4OffVoltage', 0.1) #TA4 slow turn on below ~0.8V, beware ASE? AFS 061511
#setvar('ta7OffVoltage', 0.2)

### Tapered Amplifiers Constant Current###
setvar('voltageTA1ConsCurr', 5.65)   #5.65 with constant current
setvar('voltageTA2ConsCurr', 8.77)   #8.96 with constant current
setvar('voltageTA3ConsCurr', 1.3)     #1.3 with constant current
setvar('ta4MotVoltageConsCurr', 0.304)   #0.304 with constant current
setvar('ta7MotVoltageConsCurr', 0.538)   #0.538 with constant current
setvar('voltageTA8ConsCurr', 5.58)   #5.58 with constant current

### RF Knife ###
#setvar('ddsRbResonanceFreq', 62.42606)

setvar('ddsRbResonanceFreq', 10)
# with network analyzer it's 500

setvar('microwaveCarrierFreq', 6.844682e9)
##with network analyzer it's 6.334682e9

### Atom Interferometer Lasers ###
setvar('zAxisAomMot', (100, 100, 0)) #setvar('zAxisAomMot', (200, 100, 0))
setvar('zAxisAomOff', (200, 0, 0))

setvar('braggAOM1Freq',100)
setvar('braggAOM2Freq',100)
setvar('braggAOM1MOT', (100, 100, 0))
setvar('braggAOM1Off', (100, 0, 0))

### Magnetic fields ###
setvar('varFullMagneticTrapCurrent', 250) #250 #285
#setvar('varDischargeCurrent', 35)    #35

setvar('intermediateTrapCurrent', 150) #150
setvar('intermediateTrapCurrent2', 100)

#setvar('imagingCurrentSeq', 20)
#setvar('imagingCurrent', imagingCurrentSeq)    #58
setvar('imagingCurrent', 25)    #58 or 20  33 for TOP #25
setvar('magTrapTransferCurrent', 58)

setvar('quadIsCC', False)

### Bias Coils ###

#setvar('xShiftDiffSeq', 0)
#setvar('zShiftDiff', -0.56) #-0.05
#setvar('yShiftDiff', 2) #2
#setvar('xShiftDiff', xShiftDiffSeq) #0

### 3D Bias Coils ###
setvar('xLeft', 3.17)    # 2.22 #1.66 #not plugged into scope: 2.73
setvar('xRight', 1.91)  # 1.58 #2.14 #not plugged into scope: 1.55
setvar('yLeft', 3.4)  # 1.4
setvar('yRight', 1.7)    # 3.7
setvar('zLeft', 1.72) # 3.59 (neg)
setvar('zRight', 2.21)  # 1.79
