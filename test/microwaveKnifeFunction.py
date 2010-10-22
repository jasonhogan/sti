#(170,25,0)
#(180,50,0)
#(185,75,0)
#(188,100,0)

def getKnifeDDS(knifeFreq):

    amp = 25

    if knifeFreq > 170 :
        amp = 50

    if knifeFreq > 180 :
        amp = 75

    if knifeFreq > 185 :
        amp = 100

    return (knifeFreq, amp, 0)

