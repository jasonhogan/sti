#(170,25,0)
#(180,50,0)
#(185,75,0)
#(188,100,0)

setvar('rbResonanceFrequency', 6834.682610)

##### F = 2 knife
#setvar('microwaveBaseFrequencyMHz', 7022.7)
#setvar('ddsRbResonanceFrequency', 187.99439)
#setvar('ddsMinFrequency', 90)

#### F = 1 knife
setvar('ddsRbResonanceFrequency', 100.552)
setvar('ddsMaxFrequency', 180)

def getKnifeDDSTriplet(microwaveFreq):

#### F = 2 knife
#    ddsFreq = microwaveBaseFrequencyMHz - microwaveFreq
#    if (ddsFreq > ddsRbResonanceFrequency) :
#        ddsFreq = ddsRbResonanceFrequency
#    elif (ddsFreq < ddsMinFrequency) :
#        ddsFreq = ddsMinFrequency

#### F = 1 knife
    ddsFreq = (rbResonanceFrequency - microwaveFreq) + ddsRbResonanceFrequency
    if (ddsFreq > ddsMaxFrequency) :
        ddsFreq = ddsMaxFrequency    ## avoids a large (-20 dB) cold-side spur near 6.82 GHz

    if (ddsFreq > 185) :
        amp = 100
    elif (ddsFreq > 180) :
        amp = 75
    elif (ddsFreq > 170) :
        amp = 50
    else :
        amp = 25

    return (ddsFreq, amp, 0)

def setRfCutFrequency(tStart, rfCutFreq):
    
    microwaveCutFrequency = rbResonanceFrequency - rfCutFreq
    
    event(ddsRfKnife, tStart, getKnifeDDSTriplet(microwaveCutFrequency) )

    return (tStart + 10*us)


    

