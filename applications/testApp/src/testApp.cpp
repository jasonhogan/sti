#include "testApp.h"

TestApp::TestApp(ORBManager* orb_manager, std::string configFilename) :
STI_ApplicationAdapter(orb_manager, "TestApp1", configFilename)
{
}

TestApp::~TestApp()
{
}