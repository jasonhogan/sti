### Devices ###
digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
fastAnalogOut6 = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 6)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
wavemeter=dev('AndoAQ6140', 'eplittletable.stanford.edu',7)
#spectrumAnalyzer=dev('agilentE4411bSpectrumAnalyzer',  'eplittletable.stanford.edu', 18)
vco3=dev('ADF4360-6', 'eplittletable.stanford.edu', 3)
vco0=dev('ADF4360-0', 'eplittletable.stanford.edu', 0)


### Cooling & Repump Fiber Modulator Frequency Driver - RF Switches ###
motFrequencySwitch = ch(digitalOut,2)
repumpFrequencySwitchX = ch(digitalOut,5)
sixPointEightGHzSwitch = ch(digitalOut, 16)
largeCoolingDetuningSwitch = ch(digitalOut, 17)
cmotSwitch = ch(digitalOut, 18)
### Global RF Switch Holdoffs ###
rfSwitchHoldOff = 10*us
lockAcquisitionTime = 1*ms

### Cooling & Rempump VCOs ###
coolingRb87VcoFreq = ch(vco3, 0)
coolingRb87VcoPower = ch(vco3, 1)
rempumpVcoFreq = ch(vco0, 0)
rempumpVcoPower = ch(vco0, 1)



## camera
cameraTrigger=ch(digitalOut,0)



### Probe Light AOM ###
probeLightAOM = ch(dds, 0)
### Probe Light AOM settings ###
probeAOMFreq = 110
setvar('probeLightOn', (probeAOMFreq, 100, 0) )
setvar('probeLightOff', (probeAOMFreq, 0, 0) )
dtAOMHoldoff = 10*us
### Probe Light Diagnostics ###
#absoptionLightFrequency = ch(spectrumAnalyzer, 0)


### Tapered Amplifiers ###
TA2 = ch(fastAnalogOut, 0)
TA3 = ch(fastAnalogOut, 1)


### Quadrupole Fast On/Off Circuit ###
quadCoilVoltage = ch(slowAnalogOut, 2)
quadCoilSwitch = ch(digitalOut, 6)


### Stark Shifting AOM ###
starkShiftingAOM = ch(dds,2)
### Stark Shifting Beam Settings ###
setvar('starkShiftOn', (75, 90, 0) )
setvar('starkShiftOff', (75, 0, 0) ) 
### Stark Shifting Laser ###
current1530 = ch(fastAnalogOut6,0)


### Laser Lock ###
offsetLockVco = ch(slowAnalogOut, 6)
offsetLockVcoVoltage = 6.09
lockPhaseShifter = ch(slowAnalogOut, 7)
lockPhaseShifterVoltage = 7.51
