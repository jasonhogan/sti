#include "PDMonitorApp.h"

PDMonitorApp::PDMonitorApp(ORBManager* orb_manager, std::string configFilename) :
STI_ApplicationAdapter(orb_manager, "TestApp1", configFilename)
{
}

PDMonitorApp::~PDMonitorApp()
{
}