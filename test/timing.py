# vim:fileencoding=utf_8
'''Timing related functions.

This is a dummy file used to provide a simulation of using the timing
module provided by the C-code to the embedded Python environment. With
this file, you can test out your code without the embedded environment.
'''
# Copyright (C) 2008 Olaf Mandel
# This file is part of timing-central.
#
# Timing-central is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Timing-central is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with timing-central.  If not, see <http://www.gnu.org/licenses/>.

import __main__
import inspect
_inspect=inspect
del inspect
import os
_os=os
del os
import sys
_sys=sys
del sys

# Event list.
# The list values are tupels of channel, time and new value.
_events = []

# Positions of event definitions.
# The keys of the dictionary are tupels of channel and time, the values are
# tupels of file name and line number for the event definition.
_event_pos = {}

# Measurement list.
# The list values are tupels of channel, time and description.
_measurements = []

# Positions of measurement definitions.
# The keys of the dictionary are tupels of channel and time, the values are
# tupels of file name and line number for the measurement definition.
_measurement_pos = {}

# Stack of the currently parsed files.
# This is needed to detect circular include statements.
_files_stack = []

# Set of files that were included.
# This is used to decide which files should be made read-only.
_files_used = set([])

# List of variables with overloaded values.
# The keys of the dictionary are variable names, the values are the to be
# used values (instead of the arguments in set()).
_overloads = {}

# Positions of variable definitions.
# The keys of the dictionary are variable names, the values are tupels of file
# name and line number for the variable definition.
_var_pos = {}

class dev:
    '''A hardware device providing some channels.
    '''

    def __init__(self, id, addr, module):
        if id not in ['DigOutx24 v1', 'AnOutx2 v1', 'AndorCam v1']:
            raise RuntimeError, "Unknown device type "+id
        self.id     = id
        self.addr   = addr
        self.module = module
    def __repr__(self):
        return 'dev('+repr(self.id)+','+repr(self.addr)+ \
            ','+repr(self.module)+')'
    def __str__(self):
        return str(self.addr)+'('+str(self.module)+')'
    def dt(self):
        if self.id == 'DigOutx24 v1':
            return 10e-9
        elif self.id == 'AnOutx2 v1':
            return 500e-9
        elif self.id == 'AndorCam v1':
            return 1e-6
        else:
            raise RuntimeError, "Unknown device type "+id

class ch:
    '''A hardware channel.
    '''

    def __init__(self, device, nr):
        self.device = device
        self.nr    = nr
    def __repr__(self):
        return 'ch('+repr(self.device)+','+repr(self.nr)+')'
    def __str__(self):
        return str(self.device)+':'+str(self.nr)
    def dt(self):
        return self.device.dt()

def event(channel, time, val):
    '''Create an event on a channel and time.

    The value on the channel changes instantaniously at time.
    '''

    _events.append((channel, time, val))
    frame=_sys._getframe(1)
    _event_pos[(channel,time)] = (_inspect.getfile(frame),
        _inspect.getlineno(frame))

def include(file):
    '''Reads in a file and executes it.

    The dictionary during evaluating the script is set to global.
    '''

    if file in _files_stack:
        raise RuntimeError, "Circular include: " + str(
            _files_stack[_files_stack.index(file):] + [file] )
    _files_stack.append(file)
    execfile(file, __main__.__dict__)
    _files_used.add(file)
    _files_stack.pop()

def meas(channel, time, desc=""):
    '''Schedule a measurement on a channel and time.

    The measurement is single-shot, i.e. no measurement curve is taken.
    '''

    _measurements.append((channel, time, desc))
    frame=_sys._getframe(1)
    _measurement_pos[(channel,time)] = (_inspect.getfile(frame),
        _inspect.getlineno(frame))

def setvar(name, val=None):
    '''Sets a constant to a value.

    The name of the constant must be given as a string, the resulting
    variable is put in the global context. If the variable is contained in
    the list of overwritten variables, then use that value instead of val
    to define the variable. This additionally stores the position where the
    variable was defined in timing.var_pos.
    '''

    frame = _sys._getframe(1)
    newpos = (_inspect.getfile(frame), _inspect.getlineno(frame))
    if _overloads.has_key(name):
        newval = _overloads[name]
    elif val is not None:
        newval = val
    else:
        raise RuntimeError, "Defined variable "+name+" without a value"

    if _var_pos.has_key(name):
        if (_var_pos[name],__main__.__dict__[name]) != (newpos,newval):
            raise RuntimeError, "Tried to redeclare variable "+name
        else:
            return

    __main__.__dict__[name] = newval
    _var_pos[name] = newpos
