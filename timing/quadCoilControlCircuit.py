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



def rampUpQuadCoils(time, usePreCharge = False, fullMagneticTrapCurrent = 300, chargeCurrent = 35):
     
### added from evaporativeCoolingFunction ######################################
#### Snap On Mag Field ####
#        if (usePreCharge) :
#            event(quadrupoleChargeSwitch, time, 1)
#            setQuadrupoleCurrent(time + 500*us, chargeCurrent, False, False, 0)
#            event(quadrupoleChargeSwitch, time + 0.15*ms, 0) # was 5*ms before we started monkeying with it (i.e. for 45A fast turn on)


        ### Ramp up the mag field #################################################
    time = setQuadrupoleCurrent(time + 1*ms, fullMagneticTrapCurrent, True, False, chargeCurrent, rampRate = 1)

    return time


def rampDownQuadCoils(time, dtHoldoff = 20*ms, fullMagneticTrapCurrent = 300, dischargeCurrent = 35, rapidOff = False):

### added from evaporativeCoolingFunction#######################################
    time = time + dtHoldoff

    # Let's not break things
    if (dischargeCurrent > 35)
        rapidOff = False

#    event(quadCoilShuntSwitch, time, 1)

    #### Ramp down the mag field ####
    time = setQuadrupoleCurrent(time + 1*ms, dischargeCurrent, True, False, fullMagneticTrapCurrent, rampRate = 1)

    #### Snap off the mag field ####
    tOff = internalTime
    if (rapidOff) : 
        setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
        event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
        event(quadrupoleOnSwitch, tOff, 0)

    


    #### Dummy edge to give charge capacitor time to recharge ####
#    endOfChargeTime = internalTime + 99.1*s
#    event(ch(digitalOut, 4), endOfChargeTime - 500*us, 1)
#    event(ch(digitalOut, 4), endOfChargeTime + 1*ms, 0)
###############################################################################

### Ask Dr. Johnson
#    time = time + 100*ms
    if (dischargeCurrent > 35)
        time = time + 100*ms * (dischargeCurrent - 35) / (300 - 35) 

#    tOff = time
#    setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
#    event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
#    event(quadrupoleOnSwitch, tOff, 0)
    return time
