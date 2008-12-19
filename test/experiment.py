# vim:fileencoding=utf_8
from timing import *

# Set description used by program
setvar('desc','''Dummy experiment.
This experiment file will run a MOT, then execute a detection sequence.''')

# Include needed other files
include("channels.py")

# Define different blocks of the experiment
def MOT(Start):
    '''Runs the normal MOT.

    The MOT begins at Start and the time when it ends is the return value.
    '''
    # Define local variables
    End = Start+MOT_Duration

    event(MOT_shutter,     Start, 1)
    event(MOT_shutter,     End,   0)
    event(MOTcoil_MOSFET,  Start, 1)
    event(MOTcoil_MOSFET,  End,   0)
    event(MOTcoil_current, Start, MOT_Current * MOT_curr_callib)

    return End

def Detection(Start):
    '''Detect the cloud with a camera. 

    The TOF begins at Start and the time when Detection ends is the
    return value.
    '''
    # Define local variables
    Trigger = Start+TOF_Duration
    End     = Trigger+1e-3

    event(Camera_Trigger, Trigger,                     1)
    event(Camera_Trigger, Trigger+Camera_Trigger.dt(), 0)
    meas(Camera, Trigger, "Picture 1")

    return End

# Global definitions
setvar('MOT_Current',1)      # Needs to be defined in GUI
setvar('MOT_Duration', 10)
setvar('TOF_Duration', 100e-6)
setvar('loop_values', ['S', [0.1, 0.2, 0.5], 'M', [1, 2, 5], 'L', [10, 20, 50]])
time = 0
time = MOT(time)
time = Detection(time)
