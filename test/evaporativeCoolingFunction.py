


def evaporate(tStart, dtMagneticTrap = 1*s):

    ## Quad Coil Settings ##
    magneticTrapSetpoint = 6
    quadCoilHoldOff = 1*ms
    tQuadCoilOff = tStart + dtMagneticTrap

    

    #################### events #######################

    #### Load Magnetic Trap ####
    #### Optical Pumping goes here ####
    
    
    #### Snap On Mag Field ####
    event(quadCoilVoltage, tStart - quadCoilHoldOff, magneticTrapSetpoint)

    event(quadCoilSwitch, tStart, 1)
    
    #### Ramp up the mag field ####
    
    
    #### Evaporate ####
    

    #### Ramp down the mag field ####


    #### Snap off the mag field ####
    
    event(quadCoilSwitch, tQuadCoilOff, 0)

    return tQuadCoilOff
