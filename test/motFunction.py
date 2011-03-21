


def MOT(tStart, dtMOTLoad=250*ms, leaveOn=False, tClearTime=100*ms, cMOT=True, dtSweepToCMOT=20*ms, cmotQuadCoilCurrent = 7, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 7, dtCMOT = 0*ms, powerReduction = 0.5, CMOTFrequency = 220, dtNoRepump = 0*ms, repumpAttenuatorVoltage = 10, cmotCurrentRampRate = 1):

    ## TA Settings ##
    tTAOn = tStart+tClearTime
    tTAOff =  tTAOn + dtMOTLoad


    #CMOT Settings

    ## Quad Coil Settings ##
    
    quadCoilHoldOff = 0*ms
    tQuadCoilOff = tTAOff - quadCoilHoldOff

    

    #################### events #######################

    tResetMOT = tStart

    if(tClearTime > 0) :
        ## Clear away any atoms  
        turnMOTLightOff(tResetMOT)    # TAs off

    # Initialize MOT frequency switches
    event(ch(dds,1), tResetMOT + 20*us, (ddsMotFrequency, 100, 0) )
    event(braggAOM1, tResetMOT + 40*us, (100, 100, 0))
    event(motFrequencySwitch,  tResetMOT, 0)                       # set cooling light to 10 MHz detuned via RF switch
    event(repumpFrequencySwitchX,  tResetMOT, 0)                 # set repump light on resonance via RF switch
    event(depumpSwitch, tResetMOT, 0) # switch off depumper
    event(repumpVariableAttenuator, tResetMOT + 10*us, 10) # set repump variable attenuator to max transmission

    # Initialize Quadropole Coil
    setQuadrupoleCurrent(tResetMOT, desiredCurrent = 0, applyCurrentRamp = False, usePrecharge = False)
    event(sfaOutputEnableSwitch, tResetMOT, 0)
    event(quadrupoleOnSwitch, tResetMOT, 1)
    event(quadrupoleChargeSwitch, tResetMOT, 0)

    # Turn off stray light sources
    event(probeLightShutter, tResetMOT, 0)         #close probe light shutter

    if(dtMOTLoad <= 0) :    # Load time must be greater than zero
        return tResetMOT

    ## Load the MOT ##  
    setQuadrupoleCurrent(tTAOn, desiredCurrent = motQuadCoilCurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 0)
    
    event(quadrupoleOnSwitch, tTAOn, 1)
    event(sfaOutputEnableSwitch, tTAOn + 100*us, 1)
    turnMOTLightOn(tTAOn)                            #TAs on

    if(leaveOn) :
        return tTAOn

    if(cMOT) :
        ## switch to a CMOT ##
        setQuadrupoleCurrent(tTAOff - quadCoilHoldOff - dtCMOT - dtSweepToCMOT - dtMolasses, desiredCurrent = cmotQuadCoilCurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = motQuadCoilCurrent, rampRate = cmotCurrentRampRate)
        event(ch(dds,1), tTAOff - dtCMOT - dtSweepToCMOT - dtMolasses, ((ddsMotFrequency, CMOTFrequency, dtSweepToCMOT), 100, 0) )
        
        
#        event(TA3, tTAOff  - dtCMOT - dtMolasses, powerReduction*voltageTA3)                   # TA on
#        event(TA7, tTAOff  - dtCMOT - dtMolasses, powerReduction*ta7MotVoltage)                   # TA on
        voltageSweep(channel = TA3, startTime = tTAOff - dtCMOT - 1.1*us-dtSweepToCMOT, sweepTime = dtSweepToCMOT, startVoltage = voltageTA3, stopVoltage = powerReduction*voltageTA3, numberOfEvents = 10)
        voltageSweep(channel = TA4, startTime = tTAOff - dtCMOT-dtSweepToCMOT, sweepTime = dtSweepToCMOT, startVoltage = ta4MotVoltage, stopVoltage = powerReduction*ta4MotVoltage, numberOfEvents = 10)
        

        
        if(dtNoRepump > 0) :
            event(repumpVariableAttenuator, tTAOff - dtNoRepump, repumpAttenuatorVoltage)
        
        if(dtMolasses > 0):
#            tOff = tTAOff-dtMolasses
#            setQuadrupoleCurrent(tOff-0.5*ms, 0, False, False)
#            event(sfaOutputEnableSwitch, tOff - 0.5*ms, 0)
#            event(quadrupoleOnSwitch, tOff, 0)
#            event(ch(dds,1), tOff, (139,100, 0))
            setQuadrupoleCurrent(tTAOff-dtMolasses, desiredCurrent = 30, applyCurrentRamp = True, usePrecharge = False, startingCurrent = cmotQuadCoilCurrent)
            

    ## finish loading the MOT
    turnMOTLightOff(tTAOff)        #TAs off
    event(depumpSwitch, tTAOff, 0)  # reset depump switch
    event(motFrequencySwitch, tTAOff, 0) # turn on cooling RF modulation


    if(rapidOff) :
        setQuadrupoleCurrent(tTAOff-0.1*ms, 0, False, False)
        event(sfaOutputEnableSwitch, tTAOff-0.1*ms, 0)
        event(quadrupoleOnSwitch, tTAOff, 0)


    return tTAOff


def turnMOTLightOn(tTurnOn):
    event(TA2, tTurnOn - 2.2*us, voltageTA2)                   # TA on
    event(TA3, tTurnOn - 1.1*us, voltageTA3)                   # TA on
    event(TA7, tTurnOn, ta7MotVoltage)               # TA on
    event(TA4, tTurnOn, ta4MotVoltage)
#    event(motLightShutter, tTurnOn - dtShutterBuffer, 1) ### mot shutter no longer exists - needs to be replaced with AOM functionality  DJ 3/18/2011
    return tTurnOn;

def turnMOTLightOff(tTurnOff):
    event(TA2, tTurnOff + 2.2*us, 0)                   # TA off
    event(TA3, tTurnOff + 1.1*us, 0)                   # TA off
    event(TA7, tTurnOff + 3.3*us, 0)                   # TA off
    event(TA4, tTurnOff, 0)
#    event(motLightShutter, tTurnOff + dtShutterBuffer, 0)        ### mot shutter no longer exists - needs to be replaced with AOM functionality  DJ 3/18/2011
    return tTurnOff

### Generic Functions ###

def voltageSweep(channel, startTime, sweepTime = 1.0*ms, startVoltage = 1.0, stopVoltage = 0.0, numberOfEvents = 10):
    deltaTime = sweepTime / numberOfEvents
    deltaVoltage = (stopVoltage - startVoltage) / numberOfEvents
    
    for i in range(0, numberOfEvents):
        event(channel, startTime + deltaTime*i, startVoltage + deltaVoltage*i)

    endTime = startTime + sweepTime
    return endTime
