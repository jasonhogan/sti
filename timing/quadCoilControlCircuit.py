#### Functions for using the quadrupole coil with the SFA power supply ####


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
            commandTime = commandTime + timeStepSize

            if(commandVoltage < 0):
                commandVoltage = 0

            event(sfaRemoteCurrentSetVoltage, commandTime, commandVoltage)
            deltaVoltage = fabs( commandVoltage - voltageSetpoint)

    else:
        commandTime = startTime     
        event(sfaRemoteCurrentSetVoltage, commandTime, voltageSetpoint)


    return commandTime
