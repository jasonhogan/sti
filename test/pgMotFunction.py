setvar('coolingRb87Freq', 1076)     #offset in MHz from lockpoint to 87 cooling frequency
setvar('repumpFreq', 2526)             #offset in MHz from lockpoint for the 2nd harmonic of a phase modulator to be at the 87 rempump frequency


def MOT(tStart, dtMOTLoad=250*ms, leaveOn=False, tClearTime=100*ms, cMOT=True, dtCMOT=20*ms, molasses=True, dtMolasses=10*ms):



    ## TA Settings ##
    loadVoltageTA2 = 1.45
    loadVoltageTA3 = 1.5
    cmotVoltageTA2 = 1.0
    cmotVoltageTA3 = 1.0
    molassesVoltageTA2 = 1.0
    molassesVoltageTA3 = 1.0

    #CMOT Settings
    cmotFieldMultiplier = 1

    ## Quad Coil Settings ##
    quadCoilSetting = 1.8
    cmotQuadCoilCurrent = cmotFieldMultiplier * quadCoilSetting
    quadCoilHoldOff = 1*ms

    ## time settings ##
    tTAOn = tStart+tClearTime
    tTAOff =  tTAOn + dtMOTLoad + dtCMOT * cMOT + dtMolasses * molasses
    tQuadCoilOff = tTAOff - quadCoilHoldOff
    tCmotOn = tTAOn + dtMOTLoad
    tCmotOff = tCmotOn + dtCMOT * cMOT
    tMolassesOn = tCmotOff




    #################### events #######################

    tResetMOT = tStart

    if(tClearTime > 0) :
        ## Clear away any atoms
        event(TA2, tResetMOT, 0)    # TA off MOT dark to kill any residual MOT
        event(TA3, tResetMOT, 0)    # TA off

    # Initialize MOT frequency switches
    event(motFrequencySwitch,  tResetMOT, 0)                       # set cooling light to 10 MHz detuned via RF switch
    event(repumpFrequencySwitchX,  tResetMOT, 0)                 # set repump light on resonance via RF switch

    if(cMOT or leaveOn) :
        event(coolingRb87VcoFreq, tResetMOT - 3*us, coolingRb87Freq )    # set the 87 cooling VCO to 87 cooling
        event(rempumpVcoFreq, tResetMOT + 3*us + 2.023*ms, repumpFreq)

    if(dtMOTLoad <= 0) :    # Load time must be greater than zero
        return tResetMOT

    ## Load the MOT ##  
    event(quadCoilVoltage, tTAOn, quadCoilSetting)
    event(quadCoilSwitch, tTAOn, 1)  
    event(TA2, tTAOn, loadVoltageTA2)                   # TA on
    event(TA3, tTAOn, loadVoltageTA3)                   # TA on

    if(leaveOn) :
        return tTAOn

    if(cMOT) :
        ## switch to a CMOT using RF switches ##
        event(TA2, tCmotOn - rfSwitchHoldOff, 0)                               # TA off
        event(TA3, tCmotOn - rfSwitchHoldOff, 0)                               # TA off
        event(motFrequencySwitch, tCmotOn - rfSwitchHoldOff, 1)                #set cooling light to 90 MHz detuned via RF switch
        event(repumpFrequencySwitchX, tCmotOn - rfSwitchHoldOff, 1)            #set repump light to -17dBm via RF switch
        event(TA2, tCmotOn, cmotVoltageTA2)                                      # TA on at low power for polarization gradient cooling after frequency switch has occurred
        event(TA3, tCmotOn, cmotVoltageTA3)                                      # TA on at low power for polarization gradient cooling after frequency switch has occurred
        event(quadCoilVoltage, tCmotOn - quadCoilHoldOff, cmotQuadCoilCurrent) #set quad coil current as required for the cMOT (currently no change)
    
    if(molasses) :
        event(quadCoilSwitch, tMolassesOn, 0)               # rapidly switch off the quad coils using capacitive dump circuit
        event(TA2, tMolassesOn, molassesVoltageTA2)                                      # TA on at low power for polarization gradient cooling during molasses
        event(TA3, tMolassesOn, molassesVoltageTA3)                                      # TA on at low power for polarization gradient cooling during molasses



    ## finish loading the MOT
    event(TA2, tTAOff, 0)             # TA off
    event(TA3, tTAOff, 0)             # TA off
    event(quadCoilSwitch, tQuadCoilOff, 0)      # turn off the quad coils

    return tTAOff
