#requires channels.py

generalShutterBuffer = 10*ms

def openMOTShutters(tStart):

    event(ta3SeedShutter, tStart-generalShutterBuffer, 1)
    event(motXYShutter, tStart-generalShutterBuffer, 1)
    event(motZShutter, tStart-generalShutterBuffer, 1)


def closeMOTShutters(tStart):

    event(ta3SeedShutter, tStart, 0)
    event(motXYShutter, tStart, 0)
    event(motZShutter, tStart, 0)