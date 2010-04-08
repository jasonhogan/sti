
digitalOut=dev('Digital Out','ep-timing1.stanford.edu',2)
slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)
fastAnalogOut = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 1)
fastAnalogOut6 = dev('Fast Analog Out', 'ep-timing1.stanford.edu', 6)
trigger = dev('FPGA_Trigger', 'ep-timing1.stanford.edu', 8)
dds = dev('DDS', 'ep-timing1.stanford.edu', 0)
andorCamera = dev('Andor iXon 885','ep-timing1.stanford.edu',0)
wavemeter=dev('AndoAQ6140', 'eplittletable.stanford.edu',7)
spectrumAnalyzer=dev('agilentE4411bSpectrumAnalyzer',  'eplittletable.stanford.edu', 18)


### Cooling & Repump Fiber Modulator Frequency Driver - RF Switches ###
motFrequencySwitch = ch(digitalOut,2)
repumpFrequencySwitch = ch(digitalOut,5)
### Global RF Switch Holdoffs ###
rfSwitchHoldOff = 10*us
lockAcquisitionTime = 1*ms

### Andor Camera ###
camera = ch(andorCamera, 0)

### Stark Shifting AOM ###
starkShiftingAOM = ch(dds,2)
### Stark Shifting Beam Settings ###
setvar('starkShiftOn', (75, 90, 0) )
setvar('starkShiftOff', (75, 0, 0) ) 

### Probe Light AOM ###
probeLightAOM = ch(dds, 0)
### Probe Light AOM settings ###
probeAOMFreq = 110
setvar('probeLightOn', (probeAOMFreq, 100, 0) )
setvar('probeLightOff', (probeAOMFreq, 0, 0) )
dtAOMHoldoff = 10*us



TA2 = ch(fastAnalogOut, 0)
TA3 = ch(fastAnalogOut, 1)
quadCoil = ch(slowAnalogOut, 2)
current1530 = ch(fastAnalogOut6,0)




absoptionLightFrequency = ch(spectrumAnalyzer, 0)
