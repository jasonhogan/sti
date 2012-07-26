
## The Uniblitz shutter changes state on a falling edge of the Trigger line.

## To ensure state synchronization, the Reset line must be set to LOW 
## at the start of the sequence to return the shutter to a known initial state.

## Shutter takes about 2 ms to close or open

event(bluePlugShutterReset, 10*us, 0)
event(bluePlugShutterReset, 20*us, 1)

def openBluePlugShutter(time) :
    holdoff = 3.7*ms    ## Defined so that the shutter begins to open at 'time'
    prepare = 500*us    # line must be brought HIGH before being triggered; needs about 200*us
    
    event(bluePlugShutter, time - holdoff - prepare, 1)
    event(bluePlugShutter, time - holdoff, 0)

    return

def closeBluePlugShutter(time) :
    holdoff = 4.7*ms            ## Defined so that the shutter begins to close at 'time'
    resetHoldTime = 10*us    ## How long to keep reset line HIGH
    
    event(bluePlugShutterReset, time - holdoff, 1)
    event(bluePlugShutterReset, time - holdoff + resetHoldTime, 0)

    return