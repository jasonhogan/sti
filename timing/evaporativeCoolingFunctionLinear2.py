include('microwaveKnifeFunction.py')
include('approximateExponentialSweep.py')
from math import pow

### VCAs ######################################################################
## VCA's control RF knife ramps
## ~4.5 is off. Frequencies are in MHz.
## Power should drop and ramps should get faster as you evaporate. 
## Several ramps are defined here, but not all may be executed. See evaporativeCoolingFunction
setvar('vca1', 9.5)    #6.5
setvar('f0', 120) #120
setvar('f1', 3*(4 - 0)) #60
setvar('dtRamp1', 6.0*s) #5.75

setvar('vca2', 9.5) 
setvar('f2', 3*(2))

sweepRate=(3*4 - 3*(0.01))/(1*s)
#setvar('dtRamp2', (f1-f2)/sweepRate )  #1.5*s
setvar('dtRamp2', 2*s )  #1.5*s

setvar('vca3', 9.5) #5.75
setvar('f3',3*.01)
setvar('dtRamp3', (f2-f3)/sweepRate ) #0.6*s


setvar('dtHoldCut', 0.05*s)


###############################################################################

def evaporate(tStart, rampNumber = 1):

    if (rampNumber < 1):
        return tStart

    rampList=[]
    
    #################### events #######################
    internalTime = tStart

    ### Turn microwave horn on during evaporation ######
    event(sixPointEightGHzSwitch, internalTime, 1)

    ddsRbfreq = ddsRbResonanceFreq

    evapTime = internalTime
    tEnd = evapTime
    tFrequencyRampStart=internalTime
        
    ###  Ramp 1 ###########################################
    if (rampNumber >= 1) :
        event(rfKnifeAmplitude,evapTime +  10*us, vca1)
        rampList.append((ddsRbfreq + f0, ddsRbfreq + f1, dtRamp1/s))
        tEnd = evapTime + dtRamp1
    #######################################################

#    event(rfKnifeAmplitude, evapTime +  3*s, 3)

    tRampDown=tEnd

   ### Ramp 2 ############################################
    if (rampNumber >= 2) :
        evapTime = tEnd + 10*ms
        event(rfKnifeAmplitude, evapTime - 4*ms, vca2)
        rampList.append((ddsRbfreq + f1, ddsRbfreq + f2, dtRamp2/s))
        tEnd = evapTime + dtRamp2
    #######################################################

#    tRampFinished = rampDownQuadCoils(tRampDown+1*ms, fullMagneticTrapCurrent = varFullMagneticTrapCurrent, dischargeCurrent = 150, rapidOff = False)

#    tEnd = max(tEnd, tRampFinished)    


    ### Ramp 3 ############################################
    if (rampNumber >= 3):
#        evapTime = tEnd + 20*ms
#        event(rfKnifeAmplitude, evapTime - 10*ms, vca3)
        evapTime = tEnd + 1.4*ms
        event(rfKnifeAmplitude, evapTime - 5*us, vca3)
        rampList.append((ddsRbfreq + f2, ddsRbfreq + f3, dtRamp3/s))
        tEnd = evapTime + dtRamp3
    #######################################################

    ## Run the DDS sweep for all linear segments
    event(ddsRfKnife, tFrequencyRampStart, (rampList, 100, 0) )
    


    tEnd += dtHoldCut

    event(rfKnifeAmplitude, tEnd + 0*0.1*ms, 0)
    #### Turn off microwave horn at end of evaporation)
    event(sixPointEightGHzSwitch, tEnd + 0*0.1*ms, 0)
   
    return tEnd
