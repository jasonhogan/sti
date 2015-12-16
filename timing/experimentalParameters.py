from math import fabs

ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0
urad = 0.000001

### Species ###
setvar('coolRb85', True)
setvar('coolRb87', True)
setvar('InterleavedMOT', False)

setvar('cooling87AOMvcaMaxVal', 0.0)
setvar('cooling87AOMvcaLowVal', 0.65) #0.65 for F1-F2 
setvar('fractional87Load', 0.96) #0.95 for F1-F2 #0.5

### Global RF Switch Holdoffs ###
setvar('rfSwitchHoldOff', 10*us)
setvar('lockAcquisitionTime', 1*ms)

### Cooling & Rempump VCOs ###
setvar('dds87Resonance', 129)
setvar('dds85Resonance', 97)
setvar('dds87MotFrequency', dds87Resonance + 10) #+10 #139
setvar('dds85MotFrequency', dds85Resonance + 4.) #3.2 #+3.4 #99.5; gets quadrupled
setvar('dds85DepumpFrequency', 128) #99.5; gets quadrupled 508.0/4.0

setvar('cMOT87Detuning', 45)#25 #in MHz; positive is red #46 for 87 only
setvar('cMOT85Detuning', 70) #70 #20 #40 #46#in MHz; positive is red

### Probe light shutter ###
dtShutterBuffer = 5*ms  # uncertainty in shutter timing

### Probe light RF switch settings ###
setvar('probeLightOn', 1 )
setvar('probeLightOff', 0 )
setvar('dtAOMHoldoff', 10*us)

### MOT Load Time ###

setvar('MOTLoadTime', 3.0*s) #6.0s for dual species #3.0s

### Imaging ###

# moved to the individual files:
setvar('deltaImagingFreqMHz', -0.0)  #negative detunings are easier for the lock.  For CMOT set detuning to -18, for TOP lens characterization -9

#setvar('deltaImagingFreqMHzSeq', 1.3)
#setvar('deltaImagingFreqMHz', deltaImagingFreqMHzSeq)

#setvar('ImagingTransition', "3->4")  #normal Rb85

setvar('ImagingTransition', "2->3")
#setvar('ImagingTransition', "1->2")
#setvar('ImagingTransition', "1->1")

### Evaporative Cooling Temperature ###
nK3, nK50 = range(2)
cloudTemperature = nK3

### Launch Angle ###
setvar('launchAngles', [103*urad, -0.56])  #[106*urad, -0.3]

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

############################################
############ Tapered Amplifiers ############
############################################
setvar('taZScaleFactor', 0.43*1.75) #1.75
### Fixed TAs, Intensity Locked ###
#setvar('voltageTA1', 1.224) #TA2v2 Actual
#setvar('voltageTA2', 8.77) #4.04 with intensity feedback, not used now #TA3 Actual
setvar('voltageTA3', 8.5) #7.7 #1.63 #TA6 Actual, Pit
#setvar('ta7MotVoltage', 0.475) #0.317
#setvar('voltageTA8', 2.2) #2.2

# Arroyo TAs and BOAs, see arroyoLaserFunctions.py
# [constant current (mA), intensity lock (uA)]
# or [constant current (mA)] used always
setvar('taMasterSP', [1030]) #updated 4/23/15 #TA11 3/25/15 #TA2v2 09/25/13
setvar('boaRepumpSP', [120]) #BOA2 09/25/13
setvar('ta2DPitSP', [voltageTA3]) #TA6 09/25/13
setvar('boa3DSP', [130]) #TA1 09/25/13
setvar('taRepumpSP', [1500]) #TA10 12/30/14 #TA8 09/25/13
setvar('boaLatticeSP', [110]) #BOA2 09/25/13
setvar('taPreZBoxSP', [997, 1700]) #TA9v2 09/25/13 #1063
setvar('ta87SP', [1700])
setvar('ta85SP', [1300])

### Variable TAs, Intensity Feedback ###
setvar('ta4OffVoltage', 0.039) #0.1 #0.039
setvar('ta5OffVoltage', 0.039) #0.1 #0.047
setvar('taZOffVoltage', 0.43*0.039) # #0.1 #0.047
# MOT (and repump/depump base)
setvar('taZMotVoltage', 1.4*1.2*taZScaleFactor) #1.4*1.2*taZScaleFactor #0.57 #0.849
# Lattice Catch (during TOP)
setvar('TALatticeTOPCommon', 0.6)
setvar('TALatticeTOPDiff', 0.03-0.06) #-0.03
setvar('TALatticeTOPScale', 0.6)
setvar('TA4LatticeTOPCurrent', TALatticeTOPScale*(TALatticeTOPCommon + TALatticeTOPDiff)) #.329 #0.35 #0.229
setvar('TA5LatticeTOPCurrent', TALatticeTOPScale*(TALatticeTOPCommon - TALatticeTOPDiff)) #.325 #0.511 #0.325
# Lattice Launch
setvar('TALatticeCommon', 0.7) #0.6
setvar('TALatticeDiff', -0.03) #-0.03
setvar('TALatticeScale', 1) #1 #0.7
setvar('TA4LatticeCurrent', TALatticeScale*(TALatticeCommon + TALatticeDiff)) #0.693 #1.21
#setvar('TA4LatticeCurrent', 1.09 + TALatticeCommon + TALatticeDiff) #0.693 #1.21
setvar('TA5LatticeCurrent', TALatticeScale*(TALatticeCommon - TALatticeDiff)) #0.955 #1.09

# Atom Optics Half Factors
setvar('TA4AOCurrentHalfFactor', 0.505)
setvar('TA5AOCurrentHalfFactor', 0.50)

# Atom Optics
setvar('TA4AOCurrent', 1.55) #0.725 # 1.05 #half*=0.50
setvar('TA5AOCurrent', 0.5) #1.85 #1.85 #half*=0.57

#Depump and repump
setvar('TA4DepumpVoltage', 1.0*1.3*taZScaleFactor*0.8)  #1.0*1.3*taZScaleFactor*0.8 #1*1.4*1.2*taZScaleFactor
setvar('TA4RepumpVoltage',  1.0*1.3*taZScaleFactor) #1.0*1.3*taZScaleFactor

# Blow Away
setvar('TA4F1BlowawayVoltage', 1.5*1.3*taZScaleFactor)

# Imaging
setvar('taZimagingVoltage', 1.82*taZScaleFactor) #0.98 #1.82
setvar('TAZPusherVoltage', 1.8*taZScaleFactor)
#setvar('ta4imagingVoltageUpper', 0.64)

### All TAs Constant Current (for morning tweakup) ###
#setvar('voltageTA1ConsCurr', 6.33) #TA2v2 Actual
#setvar('voltageTA2ConsCurr', 8.77) #TA3 Actual
setvar('voltageTA3ConsCurr', 1.35) #TA9 Actual
#setvar('ta7MotVoltageConsCurr', 0.4)
#setvar('voltageTA8ConsCurr', 6.52)#5.58 for doublepass TA
setvar('ta4MotVoltageConsCurr', 0.35)
setvar('voltageTA5ConsCurr', 0.35)
setvar('ta4OffVoltageConsCurr', 0.1) #TA4 slow turn on below ~0.08V, beware ASE? AFS 061511
setvar('ta5OffVoltageConsCurr', 0.1)

setvar('zAxisAOMFullPower', (100, 58.651, 0)) #was 300 before adding VCA
setvar('zAxisAomVCAFullPower', 3)

############################################
############################################

### Evaporation State ###

setvar('evapState', "F=1")

### RF Knife ###
#setvar('ddsRbResonanceFreq', 62.42606)

setvar('ddsRbResonanceFreq', 10)
setvar('dds85StatePrep', 100)
# with network analyzer it's 500


setvar('microwaveCarrierFreq', 6.844682e9) # for red knife: 6.844682e9; for blue knife 6.824682e9 
##with network analyzer it's 6.334682e9

### Atom Interferometer Lasers ###
#setvar('zAxisAomMot', (100, 100, 0)) #setvar('zAxisAomMot', (200, 100, 0))
#setvar('zAxisAomOff', (200, 0, 0))

setvar('braggAOM1Freq',100)
setvar('braggAOM2Freq',100)
setvar('braggAOM1MOT', (100, 100, 0))
setvar('braggAOM1Off', (100, 0, 0))

### Magnetic fields ###
if (evapState=="F=1") :
    setvar('varFullMagneticTrapCurrent',250) #250 #285
elif (evapState=="F=2") :
    setvar('varFullMagneticTrapCurrent', 65) #30
else :
    print 1/0
#setvar('varDischargeCurrent', 35)    #35

setvar('intermediateTrapCurrent', 90) #90 #85 #125 #150
setvar('intermediateTrapCurrent2', 60) #125 #85 #125 #100
setvar('quadJumpCurrent', 90)

setvar('topPhaseOffset', 0)

#setvar('imagingCurrentSeq', 20)
#setvar('imagingCurrent', imagingCurrentSeq)    #58
#setvar('imagingCurrentBEC', 58)    #58 or 20  33 for TOP #25
setvar('quadCurrentTOPTrap', 22.9 + 0*18.6) #22.9   #25 #18.28 #26 is the NEW 25 after fixing rounding error on 1/16/2013 #22.9
#setvar('imagingCurrent', 25)    #58 or 20  33 for TOP #25
setvar('magTrapTransferCurrent', 58) #0.5*58 #58

setvar('quadIsCC', False)

### Bias Coils ###

#setvar('xShiftDiffSeq', 0)
#setvar('zShiftDiff', -0.56) #-0.05
#setvar('yShiftDiff', 2) #2
#setvar('xShiftDiff', xShiftDiffSeq) #0

### 3D Bias Coils ###
setvar('xLeft', 2.17) #3.17 #3.5 #3.17 # 2.22 #1.66 #not plugged into scope: 2.73
setvar('xRight', 0.81)  #1.91 #1.91 # 1.58 #2.14 #not plugged into scope: 1.55
setvar('yLeft', 3.4) #3.4  #0.1 #3.4 #1.4
setvar('yRight', 1.9) #1.7   #4.9 #1.7  # 3.7
setvar('zLeft', 0.0) #0 #3 #0  #1.25  1.74 #1.7 #1.7 # 3.59 (neg)
setvar('zRight', 0.0)  #0 #1 #0  #1  #1.94 #1.9 #1.99 #1.99 # 1.79
setvar('zSwitch', 3) #2 #2.5 #1.74

#####Old Settings, for constant voltage mode
#setvar('zLeft', 1.7)  #1.7 #1.7 # 3.59 (neg)
#setvar('zRight', 1.9) #1.9lastImage = 
 #1.99 #1.99 # 1.79

### Pit scope
#pitScope = dev()
setvar('token', 1)


### Phase Matrix ###


setvar('PhaseMatrix1Power', 18.0)   #-2
setvar('PhaseMatrix2Power', 18.0)   #-2


###############################################
### Raman sideband and intensity parameters ###
###############################################
#
##Stark compensation spectrum at 3 GHz detuning
#setvar('path1RamanParameters', (0, 1.5)) # Param 1: sideband ratio; Param 2: carrier to sideband ratio
#setvar('path2RamanParameters', (0, 0.7)) # Param 1: sideband ratio; Param 2: carrier to sideband ratio



# Asymmetric Values:
#setvar('path1RamanParameters', (1.3, 0.0095)) # Param 1: sideband ratio; Param 2: carrier to sideband ratio #0.1508 #0.0095 for 1.3 #0.0165 for 0
#setvar('path2RamanParameters', (1.3, 0.0147)) # 0.1742 #0.0147 for 1.3 #0.027 for 0
#setvar('pdSetpointVoltagePath1', 0.42) #0.74 #0.55 for 1 #0.44 for 1.3
#setvar('pdSetpointVoltagePath2', 0.42) #0.74 #0.62 for 1 #0.53 for 1.3

#Symmetric Values:
setvar('path1RamanParameters', (0, 0.005)) #0.0165 # Param 1: sideband ratio; Param 2: carrier to sideband ratio #0.1508 #0.0095 for 1.3 #0.0165 for 0
setvar('path2RamanParameters', (0, 0.055)) #0.027 # 0.1742 #0.0147 for 1.3 #0.027 for 0
setvar('pdSetpointVoltagePath1', 2.2) #1.4 #1.75 #0.74#0.74 #0.55 for 1 #0.44 for 1.3
setvar('pdSetpointVoltagePath2', 1.15) #1.02 #1.4 #0.65 #0.74 #0.62 for 1 #0.53 for 1.3

setvar('topOffVoltage', 0.007)
setvar('atomOpticsAOMStartFreq', 79.5)
setvar('SSBPhaseLockFreq', 80)
#setvar('SSBStartFreq',39.5)
setvar('SSBStartFreq',40)
setvar('HFOffset',6834.68261090429)

#Physical Parameters:
setvar('gInMMperS', 9806.65-9.40)
setvar('gMeasInMMperSS', gInMMperS + 3.1)
