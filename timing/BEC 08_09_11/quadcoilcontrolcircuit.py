#### Functions for using the quadrupole coil with the SFA power supply ####
setvar('sfaCurrentSetpoint', 0)

#def setQuadrupoleCurrent(startTime, desiredCurrent = 0, applyCurrentRamp = True, usePrecharge = True, startingCurrent = 0, rampRate = 1): 
#    print 1/0
#    voltsPerAmp = 0.0165
#    offset = 0.0127
#    voltageSetpoint = desiredCurrent * voltsPerAmp - offset
#    if(voltageSetpoint < 0):
#        voltageSetpoint = 0
#
#    
#    rampScaleFactor = 2
#    timeStepSize = rampScaleFactor * 1.0*ms
#    
#    commandVoltage = startingCurrent * voltsPerAmp - offset
#    deltaVoltage = fabs( commandVoltage - voltageSetpoint)
#
#    if (deltaVoltage > 0):
#        maxVoltageStep = rampRate * rampScaleFactor * 0.028 * (voltageSetpoint - commandVoltage) / deltaVoltage
#    else:
#        maxVoltageStep = 0
#
#    if (maxVoltageStep > 0):
#        numberOfSteps = fabs((voltageSetpoint - commandVoltage) / maxVoltageStep)
#    else:
#        numberOfSteps = 0
#
##    commandTime = startTime - (numberOfSteps) * timeStepSize
#    commandTime = startTime
#
#    if(applyCurrentRamp):
#        
#        while (deltaVoltage > 0.045):
#            commandVoltage = commandVoltage + maxVoltageStep
#            commandTime = commandTime + timeStepSize
#
#            if(commandVoltage < 0):
#                commandVoltage = 0
#
#            event(sfaRemoteCurrentSetVoltage, commandTime, commandVoltage)
#            deltaVoltage = fabs( commandVoltage - voltageSetpoint)
#
#    else:
#        commandTime = startTime     
#        event(sfaRemoteCurrentSetVoltage, commandTime, voltageSetpoint)
#
#    sfaCurrentSetpoint = desiredCurrent
#    return commandTime


#def sorensenAmpsToVolts(current) :
#    voltsPerAmp = 0.03
#    offset = 0.04
#    
#    return (current * voltsPerAmp - offset)

def essAmpsToVolts(current) :
    voltsPerAmp = 0.0165
    offset = 0.0127

    volts = current * voltsPerAmp - offset
    if (volts < 0):
        volts = 0
    
    return volts

def setQuadrupoleCurrent(time, current = 0) :
    event(sfaRemoteCurrentSetVoltage, time , essAmpsToVolts(current))

def rampQuadrupoleCurrent(startTime, endTime, startCurrent = 0, endCurrent = 0, numberOfSteps = 10) :

#    startVoltage = sorensenAmpsToVolts(startCurrent)
#    endVoltage = sorensenAmpsToVolts(endCurrent)
    startVoltage = essAmpsToVolts(startCurrent)
    endVoltage = essAmpsToVolts(endCurrent)

    deltaV = 1.0 * (endVoltage - startVoltage) / numberOfSteps
    deltaT = 1.0 * (endTime - startTime) / numberOfSteps

#    print "sorensen deltaV = " + str(deltaV)
    print "ess deltaV = " + str(deltaV)

    if startVoltage < 0 or endVoltage < 0 :
        ## command voltages must be positive
        print 1/0


    if deltaT <= 0 :
        ## startTime must be before endTime
        print 1/0

    if fabs(deltaV) > 0.2 :
        ## Too large a voltage step
        print 1/0

    if fabs(deltaV / deltaT) > 3 :
        ## Too large a voltage step
        print 1/0

    ## Generate events for ramp
    for i in range(1, numberOfSteps + 1) :
        event(sfaRemoteCurrentSetVoltage, startTime + i * deltaT, startVoltage + i * deltaV)

    sfaCurrentSetpoint = endCurrent
    
    print "sfa = " + str(sfaCurrentSetpoint)
    
    return endTime


def rampUpQuadCoils(time, usePreCharge = False, fullMagneticTrapCurrent = 300, chargeCurrent = 35, quadRampRate = 1.0):
    print 1/0
### added from evaporativeCoolingFunction ######################################
#### Snap On Mag Field ####
#        if (usePreCharge) :
#            event(quadrupoleChargeSwitch, time, 1)
#            setQuadrupoleCurrent(time + 500*us, chargeCurrent, False, False, 0)
#            event(quadrupoleChargeSwitch, time + 0.15*ms, 0) # was 5*ms before we started monkeying with it (i.e. for 45A fast turn on)


    ### Ramp up the mag field #################################################
    time = setQuadrupoleCurrent(time + 1*ms, fullMagneticTrapCurrent, True, False, chargeCurrent, rampRate = quadRampRate)

    return time


def rampDownQuadCoils(time, fullMagneticTrapCurrent = sfaCurrentSetpoint, dischargeCurrent = sfaCurrentSetpoint, rapidOff = False, quadRampRate = 1.0):
    print 1/0
### added from evaporativeCoolingFunction#######################################
    # Let's not break things
    if (dischargeCurrent > 35 and rapidOff):
        rapidOff = False
        dischargeCurrent = 0

#    event(quadCoilShuntSwitch, time, 1)

    #### Ramp down the mag field ####
    time = setQuadrupoleCurrent(time, dischargeCurrent, True, False, fullMagneticTrapCurrent, rampRate = quadRampRate)

    #### Snap off the mag field ####
    tOff = time
    if (rapidOff and sfaCurrentSetpoint < 40) : 
        setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
        event(sfaOutputEnableSwitch, tOff -0.5*ms, 0)
        event(quadrupoleOnSwitch, tOff, 0)
    elif (rapidOff):
        time = 1/0

    


    #### Dummy edge to give charge capacitor time to recharge ####
#    endOfChargeTime = internalTime + 99.1*s
#    event(ch(digitalOut, 4), endOfChargeTime - 500*us, 1)
#    event(ch(digitalOut, 4), endOfChargeTime + 1*ms, 0)
###############################################################################

### Ask Dr. Johnson
#    time = time + 100*ms
    if (dischargeCurrent > 35):
        time = time + 100*ms * (dischargeCurrent - 35) / (300 - 35) 

#    tOff = time
#    setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
#    event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
#    event(quadrupoleOnSwitch, tOff, 0)
    return time

def snapOffField(tOff):
    
    if (sfaCurrentSetpoint < 60) : 
        setQuadrupoleCurrent(tOff, current = 0)
        event(quadrupoleOnSwitch, tOff, 0)
    else:
        print 1/0

    return tOff

def snapOnField(StartTime, snapCurrent = 43) :

    time = StartTime

    #rapid on
    event(quadrupoleChargeSwitch, tStart + 1*ms, 0)
    event(quadrupoleChargeSwitch, time, 1)
    event(quadrupoleChargeSwitch, time + 200*ms, 0)

    sfaHoldoff = 500*us        #4
    sfaRampTime = 0.2*ms

#    event(sfaOutputEnableSwitch, time, 0)
#    event(sfaOutputEnableSwitch, time - sfaHoldoff, 1)

#    time = rampQuadrupoleCurrent(startTime = time - sfaHoldoff, endTime = time - sfaHoldoff + sfaRampTime, startCurrent = CMOTcurrent, endCurrent = snapCurrent, numberOfSteps = 20)

    event(sfaRemoteCurrentSetVoltage, time - sfaHoldoff, essAmpsToVolts(150))

    event(sfaRemoteCurrentSetVoltage, time -.0*ms, essAmpsToVolts(snapCurrent))

##    time = rampQuadrupoleCurrent(startTime = time, endTime = time + 3*ms, startCurrent = snapCurrent, endCurrent = maxCurrent, numberOfSteps = 5)

    return time
