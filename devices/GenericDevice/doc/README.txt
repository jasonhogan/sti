Here are some sample .xml and .ini files for the GenericDevice.

The .xml files live in DeviceDescriptions. As the name suggests, these should describe the devices. Ideally, this should be done in a usage-agnostic fashion, so it can be shared by other devices in the lab, or with other labs.

The .ini files live in DeviceConfigs. This specifies connection parameters, as well as channel name overrides (though this can also be done by renaming them in the client). Usage-specific channel names are encouraged here, as these files will presumably only be used for one specific device.