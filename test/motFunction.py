


def MOT(tStart, dtMOTLoad=250*ms, leaveOn=False, tClearTime=100*ms, cMOT=True, dtSweepToCMOT=20*ms, cmotQuadCoilCurrent = 7, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = 7, dtCMOT = 0*ms, powerReduction = 0.5, CMOTFrequency = 220):

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
        event(TA2, tResetMOT, voltageTA2)    # TA2 always on
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
    event(quadrupoleOnSwitch, tResetMOT, 0)
    event(quadrupoleChargeSwitch, tResetMOT, 0)


#    if(cMOT or leaveOn) :
#        event(ch(vco3, 0), tResetMOT - 3*us, 1065.5 + 10 )    # detuned by -10 MHz 2->3'
#        event(ch(vco0 , 0), tResetMOT+3*us + 2.023*ms, 2526)

    if(dtMOTLoad <= 0) :    # Load time must be greater than zero
        return tResetMOT

    ## Load the MOT ##  
    setQuadrupoleCurrent(tTAOn, desiredCurrent = motQuadCoilCurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = 0)
    
    event(quadrupoleOnSwitch, tTAOn, 1)
    event(sfaOutputEnableSwitch, tTAOn + 100*us, 1)
    event(TA3, tTAOn, voltageTA3)                   # TA on
    event(TA7, tTAOn, ta7MotVoltage)                   # TA on

    if(leaveOn) :
        return tTAOn

    if(cMOT) :
        ## switch to a CMOT ##
        setQuadrupoleCurrent(tTAOff - quadCoilHoldOff - dtCMOT - dtMolasses, desiredCurrent = cmotQuadCoilCurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = motQuadCoilCurrent)

        event(ch(dds,1), tTAOff - dtCMOT - dtSweepToCMOT - dtMolasses, ((ddsMotFrequency, CMOTFrequency, dtCMOT), 100, 0) )
        event(TA3, tTAOff  - dtCMOT - dtMolasses, powerReduction*voltageTA3)                   # TA on
        event(TA7, tTAOff  - dtCMOT - dtMolasses, powerReduction*ta7MotVoltage)                   # TA on
        event(repumpFrequencySwitchX,  tTAOff - dtCMOT, 1)                 # turn off repump
#        event(depumpSwitch, tTAOff - dtCMOT, 1) # pump atoms into F = 1
#        event(repumpFrequencySwitchX,  tTAOff - 1.5*ms, 0)                 # turn on repump
#        event(depumpSwitch, tTAOff - 1.5*ms, 0) # pump atoms into F = 1
        
#        event(motFrequencySwitch, tTAOff - dtCMOT, 1) # set to -90 Mhz detuned
        
#        if(dtMolasses > 0):
#            event(quadrupoleOnSwitch, tTAOff - dtMolasses, 0)      # turn off the quad coils  

    ## finish loading the MOT
    event(TA3, tTAOff, 0)             # TA off
    event(TA7, tTAOff, 0)             # TA off
    event(depumpSwitch, tTAOff, 0)  # reset depump switch
    event(motFrequencySwitch, tTAOff, 0) # turn on cooling RF modulation


    if(rapidOff) :
        setQuadrupoleCurrent(tTAOff-0.1*ms, 0, False, False)
        event(sfaOutputEnableSwitch, tTAOff-0.1*ms, 0)
        event(quadrupoleOnSwitch, tTAOff, 0)


    return tTAOff
