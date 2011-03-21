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
wavemeter=dev('AndoAQ6140', 'eplittletable.stanford.edu',7)

#spectrumAnalyzer=dev('agilentE4411bSpectrumAnalyzer',  'eplittletable.stanford.edu', 18)

#vco0=dev('ADF4360-0', 'eplittletable.stanford.edu', 0)
#vco1=dev('ADF4360-5', 'eplittletable.stanford.edu', 1)
#vco2=dev('ADF4360-4', 'eplittletable.stanford.edu', 2)
#vco3=dev('ADF4360-6', 'eplittletable.stanford.edu', 3)


### 532nm Plug Shutter ###
bluePlugShutter = ch(digitalOut, 12)
bluePlugShutterReset = ch(digitalOut, 20)


### Cooling & Repump Fiber Modulator Frequency Driver - RF Switches ###
motFrequencySwitch = ch(digitalOut,2) # turns off all cooling RF
repumpFrequencySwitchX = ch(digitalOut,5)
largeCoolingDetuningSwitch = ch(digitalOut, 17)
depumpSwitch = ch(digitalOut, 18)
repumpVariableAttenuator = ch(slowAnalogOut, 4)
### Global RF Switch Holdoffs ###
rfSwitchHoldOff = 10*us
lockAcquisitionTime = 1*ms

### Cooling & Rempump VCOs ###
setvar('ddsMotFrequency',139)
#coolingRb87VcoFreq = ch(vco3, 0)
#coolingRb87VcoPower = ch(vco3, 1)
#rempumpVcoFreq = ch(vco0, 0)
#rempumpVcoPower = ch(vco0, 1)



## camera
cameraTriggerSlow=ch(slowAnalogOut,0)
iDusCameraTrigger = ch(slowAnalogOut, 5)

### Probe Light Shutter ###
probeLightShutter = ch(digitalOut, 9) ### SRS Little Table (L.T.) ch. 2 on bnc breakout ch. 9 ## DJ 3/18/2011
dtShutterBuffer = 5*ms  # uncertainty in shutter timing

### Probe Light AOM ###
#probeLightAOM = ch(dds, 0) ### no longer exists 3/18/2011
### Probe Light AOM settings ###
#probeAOMFreq = 110
#setvar('probeLightOn', (probeAOMFreq, 25, 0) )
#setvar('probeLightOff', (probeAOMFreq, 0, 0) )
### Proble Light RF Switch Settings ###
probeLightRFSwitch = ch(digitalOut, 10)
setvar('probeLightOn', 1 )
setvar('probeLightOff', 0 )

dtAOMHoldoff = 10*us
### Probe Light Diagnostics ###
#absoptionLightFrequency = ch(spectrumAnalyzer, 0)


### z-axis MOT light Shutter ###
### 2 AOMs control z-axis MOT light - currently uses TA4

### Raman Light Control ###
ramanShutter = ch(digitalOut, 22)
ramanRfSwitch = ch(digitalOut, 23)




### Optical Pumping Bias Field ###
opticalPumpingBiasfield = ch(slowAnalogOut,3)


### Tapered Amplifiers ###
TA1 = ch(slowAnalogOut, 12)
TA2 = ch(slowAnalogOut, 13)           #ch(fastAnalogOut, 0) before 5PM, 2/10/2011
TA3 = ch(slowAnalogOut, 14)           #ch(fastAnalogOut, 1) before 5PM, 2/10/2011
TA4 = ch(fastAnalogOut, 0)
TA5 = ch(fastAnalogOut, 1)
TA6 = ch(fastAnalogOut6, 0)
TA7 = ch(slowAnalogOut, 15)          #ch(fastAnalogOut6, 0) before 5PM, 2/10/2011

setvar('voltageTA1', 1.75)                   # this is only intended to be switched on/off for the evening
setvar('voltageTA2', 1.80)                 # switch off during every mag trap cycle
setvar('voltageTA3', 1.6)                   # switch off during every mag trap cycle & ramp during the cMOT
setvar('ta4MotVoltage', 0.65)             # controls a fancy power supply - this voltage setting for MOT, TA5 & TA6 are set to 0 during a MOT
setvar('ta7MotVoltage', 1.3)             # this is only intended to be switched on/off for the evening

### Rf Knife ###
sixPointEightGHzSwitch = ch(digitalOut, 16)
ddsRfKnife = ch(dds, 3)
#hp83711b = dev('repump hp83711b', 'eplittletable.stanford.edu', 16)
#hp83711b = dev('hp83711bStandaloneDevice', 'li-gpib.stanford.edu', 16)
#rfKnifeFrequency = ch(hp83711b, 2)
rfKnifeAmplitude = ch(slowAnalogOut, 8)


### Quadrupole Fast On/Off Circuit ###
sfaRemoteCurrentSetVoltage = ch(slowAnalogOut, 2)
sfaOutputEnableSwitch = ch(digitalOut, 19)

quadrupoleOnSwitch = ch(digitalOut, 6)        #controls the IGBT in the qual coil electronics box
quadrupoleChargeSwitch = ch(digitalOut, 17)    #controls the IGBT for the charge circuit

def setQuadrupoleCurrent(startTime, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = True, startingCurrent = 0, rampRate = 1): 
    voltsPerAmp = 0.03
    offset = 0.04
    voltageSetpoint = desiredCurrent * voltsPerAmp - offset
    if(voltageSetpoint < 0):
        voltageSetpoint = 0

    
    rampScaleFactor = 2
    timeStepSize = rampScaleFactor * 1.0*ms
    
    commandVoltage = startingCurrent * voltsPerAmp - offset
    deltaVoltage = fabs( commandVoltage - voltageSetpoint)

    if (deltaVoltage > 0):
        maxVoltageStep = rampRate * rampScaleFactor * 0.028 * (voltageSetpoint - commandVoltage) / deltaVoltage
    else:
        maxVoltageStep = 0

    if (maxVoltageStep > 0):
        numberOfSteps = fabs((voltageSetpoint - commandVoltage) / maxVoltageStep)
    else:
        numberOfSteps = 0

#    commandTime = startTime - (numberOfSteps) * timeStepSize
    commandTime = startTime

    if(applyCurrentRamp):
        
        while (deltaVoltage > 0.045):
            commandVoltage = commandVoltage + maxVoltageStep

#            if (commandVoltage > voltageSetpoint) :
#                commandTime = (timeStepSize * (1 - (commandVoltage - voltageSetpoint) / maxVoltageStep)) + commandTime 
#                commandVoltage = voltageSetpoint
#            else : 

            commandTime = commandTime + timeStepSize

            if(commandVoltage < 0):
                commandVoltage = 0

            event(sfaRemoteCurrentSetVoltage, commandTime, commandVoltage)
            deltaVoltage = fabs( commandVoltage - voltageSetpoint)

    else:
        commandTime = startTime     
        event(sfaRemoteCurrentSetVoltage, commandTime, voltageSetpoint)


    return commandTime
     



### Stark Shifting AOM ###
#starkShiftingAOM = ch(dds,2)
### Stark Shifting Beam Settings ###
setvar('starkShiftOn', (75, 90, 0) )
setvar('starkShiftOff', (75, 0, 0) ) 
### Stark Shifting Laser ###


### Laser Lock ###
offsetLockVco = ch(slowAnalogOut, 6)
offsetLockVcoVoltage = 6.09
lockPhaseShifter = ch(slowAnalogOut, 7)
lockPhaseShifterVoltage = 7.51

### Atom Interferometer Lasers ###
braggAOM1 = ch(dds,0)
braggAOM2 = ch(dds, 2)
#braggAOM3 = ch(dds, 3)




    
    
