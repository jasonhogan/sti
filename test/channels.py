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
motFrequencySwitch = ch(digitalOut,2) # turns off all cooling RF
repumpFrequencySwitchX = ch(digitalOut,5)
sixPointEightGHzSwitch = ch(digitalOut, 16)
largeCoolingDetuningSwitch = ch(digitalOut, 17)
depumpSwitch = ch(digitalOut, 18)
### Global RF Switch Holdoffs ###
rfSwitchHoldOff = 10*us
lockAcquisitionTime = 1*ms

### Cooling & Rempump VCOs ###
setvar('ddsMotFrequency',139)
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
TA7 = ch(fastAnalogOut6, 0)
setvar('voltageTA2', 1.65)
setvar('voltageTA3', 1.5)
setvar('ta7MotVoltage', 0.92)   ### 0.92


### Quadrupole Fast On/Off Circuit ###
sfaRemoteCurrentSetVoltage = ch(slowAnalogOut, 2)
sfaOutputEnableSwitch = ch(digitalOut, 19)

quadrupoleOnSwitch = ch(digitalOut, 6)
quadrupoleChargeSwitch = ch(digitalOut, 17)    

def setQuadrupoleCurrent(startTime, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = True, startingCurrent = 0): 
    voltsPerAmp = 0.03
    offset = 0.04
    voltageSetpoint = desiredCurrent * voltsPerAmp - offset
    if(voltageSetpoint < 0):
        voltageSetpoint = 0

    
    timeStepSize = 1.0*ms
    maxVoltageStep = 0.028
    commandVoltage = startingCurrent * voltsPerAmp - offset
    numberOfSteps = (voltageSetpoint - commandVoltage) / maxVoltageStep
#    commandTime = startTime - (numberOfSteps) * timeStepSize
    commandTime = startTime

    if(applyCurrentRamp):
        
        while (commandVoltage < voltageSetpoint):
            commandVoltage = commandVoltage + maxVoltageStep

            if (commandVoltage > voltageSetpoint) :
                commandTime = (timeStepSize * (1 - (commandVoltage - voltageSetpoint) / maxVoltageStep)) + commandTime 
                commandVoltage = voltageSetpoint
            else : 
                commandTime = commandTime + timeStepSize

            if(commandVoltage < 0):
                commandVoltage = 0

            event(sfaRemoteCurrentSetVoltage, commandTime, commandVoltage)

    else:
        commandTime = startTime     
        event(sfaRemoteCurrentSetVoltage, commandTime, voltageSetpoint)


    return commandTime
     






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
