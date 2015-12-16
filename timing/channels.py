from math import fabs



ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0




### Devices ###
digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
fastAnalogOut6 = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 6)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
dds7 = dev('DDS', 'ep-timing1.stanford.edu', 7)


digitalOutTS2=dev('Digital Out','ep-timing2.stanford.edu',2)
slowAnalogOutTS2=dev('Slow Analog Out', 'ep-timing2.stanford.edu', 4)
fastAnalogOutTS2 = dev('Fast Analog Out', 'ep-timing2.stanford.edu', 1)
fastAnalogOut6TS2 = dev('Fast Analog Out', 'ep-timing2.stanford.edu', 6)
triggerTS2 = dev('FPGA_Trigger', 'ep-timing2.stanford.edu', 8)
ddsTS2 = dev('DDS', 'ep-timing2.stanford.edu', 0)
dds7TS2 = dev('DDS', 'ep-timing2.stanford.edu', 7)

TS2externalTrigger = ch(digitalOut, 20)

wavemeter=dev('AndoAQ6140', 'eplittletable.stanford.edu',7)
masterVortex=dev('Vortex6000', 'eplittletable.stanford.edu', 1)
laserLock = dev('Lock', '171.64.56.254', 1)
mcl = dev('MCL NanoDrive','epdesktop1.stanford.edu', 0)
phaseMatrix = dev('PhaseMatrix 2', 'EPMezzanine1.stanford.edu', 1)
phaseMatrix1 = dev('PhaseMatrix', 'EPMezzanine1.stanford.edu', 0)

spectrumAnalyzer=dev('agilentE4411bSpectrumAnalyzer',  'eplittletable.stanford.edu', 18)

littleTableNovatech=dev('Little Table Novatech', 'eplittletable.stanford.edu', 0)
zAxisAOM = ch(littleTableNovatech, 1)

highPowerLaserIntensityLockPath1 = dev('High Power Intensity Lock 1', 'epmezzanine1.stanford.edu', 1)
highPowerLaserIntensityLockPath2 = dev('High Power Intensity Lock 2', 'epmezzanine1.stanford.edu', 2)
hpSidebandLock1 = dev("High Freq Sideband Lock 1", "epmezzanine1.stanford.edu", 1)
hpSidebandLock2 = dev("High Freq Sideband Lock 2", "epmezzanine1.stanford.edu", 2)
pplnOven1=dev('PPLN Temp Arroyo 1','EPMezzanine1.stanford.edu',8)
pplnOven2=dev('PPLN Temp Arroyo 2','EPMezzanine1.stanford.edu',7)

AWGDevice=dev('ArbFuncGenerator', '171.64.56.226', 5)

muxAnalogIn0 = dev('MUX Analog In 0','epmezzanine1.stanford.edu', 0)

epscope2 = dev('epscope2', 'epmezzanine1.stanford.edu', 0)

nanoDrive=dev('MCL NanoDrive', 'epdesktop1.stanford.edu', 0)

pitNovatech=dev('Pit Novatech','171.64.56.96',0)

#Novatech Channels
topQuadratureLO=ch(pitNovatech,0)
topQuadratureRF=ch(pitNovatech,1)

#RCS Mirror
mirrorPosition=ch(nanoDrive,0)
measMirrorPosition=ch(nanoDrive,11)

# TAs and BOAs
taMaster = dev('TA Master', 'eplittletable.stanford.edu', 15)
boaRepump = dev('BOA Repump', 'eplittletable.stanford.edu', 13)
#ta2DPit
ta87 = dev('TA 87', 'eplittletable.stanford.edu', 19)
taRepump = dev('TA Repump', 'eplittletable.stanford.edu', 16)
boaLattice = dev('BOA Lattice', 'eplittletable.stanford.edu', 14)
taPreZBox = dev('TA PreZBox', 'eplittletable.stanford.edu', 18)
ta85 = dev('TA 85', 'eplittletable.stanford.edu', 20)

#vco0=dev('ADF4360-0', 'eplittletable.stanford.edu', 0)
#vco1=dev('ADF4360-5', 'eplittletable.stanford.edu', 1)
#vco2=dev('ADF4360-4', 'eplittletable.stanford.edu', 2)
#vco3=dev('ADF4360-6', 'eplittletable.stanford.edu', 3)

masterLockCheck = ch(laserLock, 0)
imagingLockCheck = ch(laserLock, 1)

digitalSynch=ch(digitalOut, 4)
fabryPerotTrigger=ch(digitalOut, 19)


### Cooling & Repump Fiber Modulator Frequency Driver - RF Switches ###
motFrequencySwitch = ch(digitalOut,2) # turns off all cooling RF

depumpSwitch = ch(digitalOut, 18)
repumpVariableAttenuator = ch(slowAnalogOut, 4)
repumpSwitch = ch(digitalOut, 3)                                # 0 is off, 1 is on

### Switch Cooling over to HP for Lattice Alignment ###
coolingSourceSwitch = ch(digitalOut, 22)

### Cooling & Rempump VCOs ###
#coolingRb87VcoFreq = ch(vco3, 0)
#coolingRb87VcoPower = ch(vco3, 1)
#rempumpVcoFreq = ch(vco0, 0)
#rempumpVcoPower = ch(vco0, 1)

### 532nm Plug Shutter ###
#bluePlugShutter = ch(digitalOut, 12)
#bluePlugShutterReset = ch(digitalOut, 20)

## camera
cameraTriggerSlow=ch(slowAnalogOut,0)
#iDusCameraTrigger = ch(slowAnalogOut, 5)

### Probe Light Shutter ###
probeLightShutter = ch(digitalOut, 9) ### SRS Little Table (L.T.) ch. 2 on bnc breakout ch. 9 ## DJ 3/18/2011


### Probe Light RF Switch Settings ###
probeLightRFSwitch = ch(digitalOut, 10)

#hp83711b = dev('repump hp83711b', 'eplittletable.stanford.edu', 16)
#imagingOffsetFrequency = ch(hp83711b, 2)

novatechChannelPair=dev('Imaging Detuning','eplittletable.stanford.edu',0)
imagingDetuning = ch (novatechChannelPair, 0)


### Probe Light Diagnostics ###
#absoptionLightFrequency = ch(spectrumAnalyzer, 0)


### z-axis MOT light Shutter ###
### 2 AOMs control z-axis MOT light - currently uses TA4

#### Raman Light Control ###

#ramanRfSwitch = ch(digitalOut, 23)




### Optical Pumping Bias Field ###
#opticalPumpingBiasfield = ch(slowAnalogOut,3)


### Tapered Amplifiers ###
#TA1 = ch(slowAnalogOut, 12)
#TA2 = ch(slowAnalogOut, 13)           #ch(fastAnalogOut, 0) before 5PM, 2/10/2011
TA3 = ch(slowAnalogOut, 14)           #ch(fastAnalogOut, 1) before 5PM, 2/10/2011
TA4 = ch(fastAnalogOut, 0)
TA5 = ch(fastAnalogOut, 1)
#TAZ = ch(slowAnalogOut, 17)
#TA6 = ch(fastAnalogOut6, 0)
rf85Switch = ch(slowAnalogOut, 15)          #ch(fastAnalogOut6, 0) before 5PM, 2/10/2011
#TA8 = ch(slowAnalogOut, 9)

### Rf Knife ###
sixPointEightGHzSwitch = ch(digitalOut, 16)
#ddsRfKnife = ch(dds, 3)
hp83712a = dev("repump hp83712a","li-gpib.stanford.edu",5)
hpMicrowaveCarrier = ch(hp83712a, 2)

#spectrumAnalyzer4395A = dev("Network Analyzer 4395A", "eplittletable.stanford.edu", 10) #spectrum analyzer sweep
ddsRfKnife = ch(dds, 2)
rf85DDS = ch(dds7TS2, 2)

#hp83711b = dev('repump hp83711b', 'eplittletable.stanford.edu', 16)
#hp83711b = dev('hp83711bStandaloneDevice', 'li-gpib.stanford.edu', 16)
#rfKnifeFrequency = ch(hp83711b, 2)
rfKnifeAmplitude = ch(slowAnalogOut, 8)
microwave85VCA = ch(slowAnalogOut, 17)

### Quadrupole Fast On/Off Circuit ###
sfaRemoteCurrentSetVoltage = ch(slowAnalogOut, 2)
sfaRemoteVoltageSetVoltage = ch(slowAnalogOut, 6)

sfaOutputEnableSwitch = ch(digitalOut, 19)

quadrupoleOnSwitch = ch(digitalOut, 6)        #controls the IGBT in the qual coil electronics box
quadrupoleChargeSwitch = ch(digitalOut, 17)    #controls the IGBT for the charge circuit


#quadCoilShuntSwitch = ch(digitalOut, 21)  # temporarily used for 85Rb; unplugged


### Atom Interferometer Lasers ###
zAxisRfSwitch = ch(digitalOut, 1)    #for doubled 100 MHz signal driving single-pass AOM (at 200 MHz)
#zAxisAom = ch(dds,2) OLD


braggAOM1 = ch(dds,0)            #TA4
braggAOM2 = ch(dds, 3)           #TA5

braggPhaseMod = ch(dds7,0)


#phaseMatrix1Trigger = ch(slowAnalogOut, 25)
atomOpticsAOM = ch(dds7, 0)
atomOpticsSSB = ch(dds7TS2, 1)

#atomOpticsSSB = ch(dds7,1)

##### MOT Light Shutters #####

ta3SeedShutter = ch(digitalOut, 8)
twoDMOTShutter = ch(digitalOut, 7)
motZShutter = ch(digitalOut, 15)
#ramanTA4Shutter=ch(slowAnalogOut, 33)
ImagingLatticeShutter=ch(slowAnalogOut, 33)

finalZShutter=ch(digitalOut, 21)
ramanPulseGate=ch(digitalOut, 22)
phaseLockBypass=ch(slowAnalogOut, 29) #5V=bypass, 0V=optical beatnote

#fiberLaserPowerSet1=ch(fastAnalogOut6, 0)
#fiberLaserPowerSet2=ch(fastAnalogOut6, 1)
#fiberLaserPowerAttenuator1=ch(slowAnalogOut, 30)
#fiberLaserPowerAttenuator2=ch(slowAnalogOut, 31)


fiberLaserPowerSet1=ch(slowAnalogOut, 1)
fiberLaserPowerSet2=ch(slowAnalogOut, 3)
fiberLaserPowerAttenuator1=ch(fastAnalogOut6, 0)
fiberLaserPowerAttenuator2=ch(fastAnalogOut6, 1)

AWGTrigger = ch(slowAnalogOut, 32)

AWGFreq = ch(AWGDevice, 0)

sidebandPowerSetpointPath1=ch(slowAnalogOut, 10)

hpLaserPDMUXSelect=ch(slowAnalogOut, 34)

cooling87Shutter = ch(digitalOut, 14)
cooling87AOMvca = ch(slowAnalogOut, 13)
repumpShutter = ch(digitalOut, 11)
latticeShutter = ch(digitalOut, 0)
ta7LatticeShutter = ch(digitalOut,13)
ta7MOTShutter = ch(digitalOut,23)

f1BlowawayShutter = ch(slowAnalogOut, 23)
f2Blowaway85Switch = ch(slowAnalogOut, 7)

##### 3D Z Bias Coils #####

zBiasTop = ch(slowAnalogOut, 19)
zBiasBot = ch(slowAnalogOut, 20)
yBiasLeftDevice = dev("E3642a 3D:Y:L","171.64.56.96", 5)
yBiasLeft = ch(yBiasLeftDevice, 0)
yBiasRightDevice = dev("E3642a 3D:Y:R","171.64.56.96", 10)
yBiasRight = ch(yBiasRightDevice, 0)
xBiasLeftDevice = dev("E3642a 3D:X:L","171.64.56.96", 8)
xBiasLeft = ch(xBiasLeftDevice, 0)
xBiasRightDevice = dev("E3642a 3D:X:R","171.64.56.96",9)
xBiasRight = ch(xBiasRightDevice, 0)

zBiasTopRelay = ch(slowAnalogOut, 5)


##### Top Coils #####
topSafetySwitch = ch(digitalOut, 5)
topVCA = ch(slowAnalogOut, 16)
topLockSwitch = ch(digitalOut, 12)

### Pit Scope ###
pitScope = dev('Pit Scope', 'Trident.stanford.edu', 0)
pitNovatechTrigger=ch(slowAnalogOut,24)

### TA4/TA5 Mach Zehnder demodulation ###
#machZehnderAODDS = ch(dds7, 0)

### Atom Interferometry Beams Phase Lock ###

srs7Set = ch(slowAnalogOut, 31) #sets external reference for SRS 7

### Detection ###

detectionWaveplateFlipMountTrigger = ch(digitalOut, 20)

### F=1 Optical Pumping ###

opticalPumpingRFPowerControl = ch(slowAnalogOut, 18)
opticalPumpingShutter = ch(slowAnalogOut, 12)
opticalPumpingAgiltron = ch(slowAnalogOut, 7)

############################################################
################ 85 Rb only channels #######################
############################################################

cooling85Switch = ch(slowAnalogOut, 21) #RF switch to turn on 85 cooling modulation (LOW is 85 cooling; HIGH is 85 depump)
rb85FreqSwitch = ch(slowAnalogOut, 22) #RF switch to turn on 85 Rb fibermod modulation (LOW is modulation on; HIGH is modulation off)
cooling85Shutter = ch(slowAnalogOut, 26) #Optical shutter to block seed to the cooling 85 fibermod. (LOW is shutter closed; HIGH is shutter open)

cooling85DDS = ch(dds7, 2) # Gets quadrupled. 97 is resonance. Acceptable range is 90 to 115 MHz

#rcsYaxis=ch(slowAnalogOut, 29)

#taZAgiltronSwitch = ch(slowAnalogOut, 32)  #0 for MOT path, 5 for other path


