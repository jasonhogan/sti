


def evaporate(tStart, dtMagneticTrap = 1*s, magneticTrapSetpoint = 20, rapidOff = False, cmotCurrent = 7):

    ## Quad Coil Settings ##
    
    quadCoilHoldOff = 0.5*ms
    tQuadCoilOff = tStart + dtMagneticTrap

    

    #################### events #######################

    #### Load Magnetic Trap ####
    #### Optical Pumping goes here ####
    
    
    #### Snap On Mag Field ####
#    event(quadCoilVoltage, tStart - quadCoilHoldOff, magneticTrapSetpoint)
#
#    event(quadCoilSwitchOff, tStart, 1)
#    event(quadCoilSwitchOn, tStart, 1)      # turn on the high voltage kick for the quad coils
#    event(quadCoilSwitchOn, tStart + 20*ms, 0)      # turn off the high voltage kick for the quad coils
    
    #### Ramp up the mag field ####
    setQuadrupoleCurrent(tStart + dtMagneticTrap, magneticTrapSetpoint, True, False, startingCurrent = cmotCurrent)
    
    #### Evaporate ####
    

    #### Ramp down the mag field ####


    #### Snap off the mag field ####
    
    if(rapidOff) :
        setQuadrupoleCurrent(tQuadCoilOff-0.1*ms, 0, False, False)
        event(sfaOutputEnableSwitch, tQuadCoilOff-0.1*ms, 0)
        event(quadrupoleOnSwitch, tQuadCoilOff, 0)


    return tQuadCoilOff
