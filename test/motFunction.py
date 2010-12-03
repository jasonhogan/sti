


def MOT(tStart, dtMOTLoad=250*ms, leaveOn=False, tClearTime=100*ms, cMOT=True, dtSweepToCMOT=20*ms, cmotQuadCoilCurrent = 7, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 7, dtCMOT = 0*ms, powerReduction = 0.5, CMOTFrequency = 220, dtNoRepump = 0*ms):

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
        event(TA2, tResetMOT, 0)    # TA off
        event(TA3, tResetMOT, 0)    # TA off
        event(TA7, tResetMOT, 0)    # TA off

    # Initialize MOT frequency switches
    event(ch(dds,1), tResetMOT + 20*us, (ddsMotFrequency, 100, 0) )
    event(motFrequencySwitch,  tResetMOT, 0)                       # set cooling light to 10 MHz detuned via RF switch
    event(repumpFrequencySwitchX,  tResetMOT, 0)                 # set repump light on resonance via RF switch
    event(depumpSwitch, tResetMOT, 0) # switch off depumper

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
    event(TA2, tTAOn, voltageTA2)                   # TA on
    event(TA3, tTAOn, voltageTA3)                   # TA on
    event(TA7, tTAOn, ta7MotVoltage)                   # TA on

    if(leaveOn) :
        return tTAOn

    if(cMOT) :
        ## switch to a CMOT ##
        setQuadrupoleCurrent(tTAOff - quadCoilHoldOff - dtCMOT - dtMolasses, desiredCurrent = cmotQuadCoilCurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = motQuadCoilCurrent)

        event(ch(dds,1), tTAOff - dtCMOT - dtSweepToCMOT - dtMolasses, ((ddsMotFrequency, CMOTFrequency, dtSweepToCMOT), 100, 0) )
        event(TA3, tTAOff  - dtCMOT - dtMolasses, powerReduction*voltageTA3)                   # TA on
        event(TA7, tTAOff  - dtCMOT - dtMolasses, powerReduction*ta7MotVoltage)                   # TA on
#        event(motFrequencySwitch, tTAOff  - dtCMOT - dtMolasses, 1) # switch in far detuned VCO
        
        if(dtNoRepump > 0) :
            event(repumpFrequencySwitchX,  tTAOff - dtNoRepump, 1)                 # turn off repump
        

#        event(depumpSwitch, tTAOff - dtCMOT, 1) # pump atoms into F = 1
#        event(repumpFrequencySwitchX,  tTAOff - 1.5*ms, 0)                 # turn on repump
#        event(depumpSwitch, tTAOff - 1.5*ms, 0) # pump atoms into F = 1
        
#        event(motFrequencySwitch, tTAOff - dtCMOT, 1) # set to -90 Mhz detuned
        
        if(dtMolasses > 0):
#            event(quadrupoleOnSwitch, tTAOff - dtMolasses, 0)      # turn off the quad coils 
            setQuadrupoleCurrent(tTAOff-dtMolasses, 0, False, False)

    ## finish loading the MOT
    event(TA2, tTAOff, 0)             # TA off
    event(TA3, tTAOff, 0)             # TA off
    event(TA7, tTAOff, 0)             # TA off
    event(depumpSwitch, tTAOff, 0)  # reset depump switch
    event(motFrequencySwitch, tTAOff, 0) # turn on cooling RF modulation


    if(rapidOff) :
        setQuadrupoleCurrent(tTAOff-0.1*ms, 0, False, False)
        event(sfaOutputEnableSwitch, tTAOff-0.1*ms, 0)
        event(quadrupoleOnSwitch, tTAOff, 0)


    return tTAOff
