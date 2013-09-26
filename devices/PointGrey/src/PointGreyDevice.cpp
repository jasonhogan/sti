#include "PointGreyDevice.hpp"

//#define PRINTF_DEBUG
#ifdef PRINTF_DEBUG
#define DEBUGHERE cerr << __FUNCTION__ << " (" << __LINE__ << ")" << endl
#define IMPLEMENT cerr << "Implement (if needed): " <<  __FUNCTION__ << "() in " << __FILE__ << ":" << __LINE__ << endl
#define FIXME(feature) cerr << "FIXME: " << feature << __FILE__ << ":" << __LINE__ << " [" << __FUNCTION__ << "]" << endl
#define TODO(something) cerr << "TODO: " << something << __FILE__ << ":" << __LINE__ << " [" << __FUNCTION__ << "]" << endl
#define ADD_FEATURE(feature) cerr << "TODO: " << feature << __FILE__ << ":" << __LINE__ << " [" << __FUNCTION__ << "]" << endl
#define DEBUG(msg) cerr << __FUNCTION__ << "(): " << msg << endl
#else
#define DEBUGHERE // Do nothing if PRINTF_DEBUG isn't defined
#define IMPLEMENT
#define FIXME(feature)
#define TODO(something)
#define ADD_FEATURE(feature)
#define DEBUG(msg)
#endif

using namespace std;
using namespace FlyCapture2;

PointGreyDevice::PointGreyDevice(ORBManager* orb_manager,
                             string DeviceName,
                             string Address,
                             unsigned short ModuleNumber, // Ugh, if this were unsigned int we could fit the serial #...
                             unsigned int serialNo,
                             string logDirectory) :
STI_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory)
{
	initialized = false;
	cameraConnected = false;
	cameraCapturing = false;
	Error error;
	// Kindly given to us by the Point Grey SDK
	camera = new Camera();

	BusManager busMgr;
	unsigned int numCameras;
	error = busMgr.GetNumOfCameras(&numCameras);
	if (wasError(error)) return;
	
	DEBUG("Number of cameras connected: " << numCameras);

	error = busMgr.GetCameraFromSerialNumber(serialNo, &guid);
	if (wasError(error)) {
		cerr << "Uh-oh, could not find camera with S/N "
		     << serialNo << endl;

		return;
	}
	
	// Connect to a camera
	error = camera->Connect(&guid);
	if (wasError(error)) return;

	cameraConnected = true;

	error = camera->GetCameraInfo(&camInfo);
	if (wasError(error)) return;

	// Color, if available
	if (camInfo.isColorCamera)
		pxfmt = PIXEL_FORMAT_RGB;
	else
		pxfmt = PIXEL_FORMAT_MONO8;

	TriggerMode trig;
	trig.onOff = true;

	camera->SetTriggerMode(&trig);

	// Start capturing images, since we're triggered
	error = camera->StartCapture();
	if (wasError(error)) return;
	
	cameraConnected = true;
	initialized = true;
}

PointGreyDevice::~PointGreyDevice()
{
	TODO("Verify we're all cleaned up!");

	// Stop capturing images
	if (cameraCapturing)
		wasError(camera->StopCapture());

	// Disconnect the camera
	if (cameraConnected)
		wasError(camera->Disconnect());

	free(camera);
}

bool PointGreyDevice::wasError(Error &error)
{
	if (error == PGRERROR_OK)
		return false;

	error.PrintErrorTrace();

	return true;
}

void PointGreyDevice::defineChannels()
{
	addInputChannel(0, DataVector, ValueVector, "Pic");

	return;
}

bool PointGreyDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	Error error;
	Image rawImage, convertedImage;

	error = camera->FireSoftwareTrigger();
	if (wasError(error)) return false;

	error = camera->RetrieveBuffer(&rawImage);
	if (wasError(error)) return false;

	error = rawImage.Convert(pxfmt, &convertedImage);
	if (wasError(error)) return false;

	// Deal with filename
	string filename = valueIn.getString();

	if (filename.empty()) {
		filename = PG_DEFAULT_FILENAME;
	}

	// Save the image. If a file format is not passed in, then the file
	// extension is parsed to attempt to determine the file format.
	error = convertedImage.Save(filename.c_str());
	if (wasError(error)) return false;

	return true;
}

void PointGreyDevice::defineAttributes()
{
	string selectedPxFmt;
	if (pxfmt == PIXEL_FORMAT_RGB)
		selectedPxFmt = ATTR_VAL_PXFMT_COLOR;
	else
		selectedPxFmt = ATTR_VAL_PXFMT_BW;

	addAttribute(ATTR_KEY_PXFMT, selectedPxFmt,
	                             ATTR_VAL_PXFMT_BW ","
	                             ATTR_VAL_PXFMT_COLOR);
}

bool PointGreyDevice::updateAttribute(std::string key, std::string value)
{
	// TODO: Sort out available modes, display all of 'em I guess?
	if (key == ATTR_KEY_PXFMT) {
		if (value == ATTR_VAL_PXFMT_BW)
			pxfmt = PIXEL_FORMAT_MONO8;
		else if (value == ATTR_VAL_PXFMT_COLOR)
			pxfmt = PIXEL_FORMAT_RGB;
		else
			return false;

		return true;
	}

	return false;
}

void PointGreyDevice::parseDeviceEvents(const RawEventMap& eventsIn,
                                      SynchronousEventVector& eventsOut)
                                     throw(std::exception)
{
	parseDeviceEventsDefault(eventsIn, eventsOut);
}