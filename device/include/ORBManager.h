/*! \file ORBManager.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ORBManager
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ORBMANAGER_H
#define ORBMANAGER_H

#ifndef __CORBA_H_EXTERNAL_GUARD__
#include <omniORB4/CORBA.h>
#endif

#include <vector>
#include <string>
#include <sstream>

class COSBindingNode;

class ORBManager
{
public:

	ORBManager(int& argc, char** argv,
				  const char* orb_identifier="",
				  const char* options[][2]=0);
	~ORBManager();

	std::string errMsg() const;
	void run();
	void waitForRun();
	void ORBshutdown();

	bool isRunning();

	int getArgc() const;
	char** getArgv() const;

	CORBA::Boolean bindObjectToName(CORBA::Object_ptr objref, 
		std::string objectStringName);
	CORBA::Object_ptr getObjectReference(std::string objectStringName);

	void printObjectTree(std::string context);
	
	COSBindingNode getCOSBindingNode(std::string name, std::string context);

	CosNaming::NamingContext_var getNamingContext(std::string context);

	template<typename T> bool registerServant(T* servant, std::string objectStringName)
	{
		bool success = false;

		servantRegistrationMutex->lock();
		{
			try {
//				poa->activate_object(servant);
				
				//get a reference to servant and bind it to the NameService
				servantRefs.push_back(servant->_this());
				bindObjectToName(servantRefs.back(), objectStringName);

				// remove the reference to servant
				servant->_remove_ref();
				success = true;
			}
			catch(CORBA::SystemException& ex) {
				errStream << "Caught CORBA::" << ex._name() << std::endl;
			}
			catch(CORBA::Exception& ex) {
				errStream << "Caught CORBA::Exception: " << ex._name() << std::endl;
			}
			catch(omniORB::fatalException& fe) {
				errStream << "Caught omniORB::fatalException:" << std::endl;
				errStream << "  file: " << fe.file() << std::endl;
				errStream << "  line: " << fe.line() << std::endl;
				errStream << "  mesg: " << fe.errmsg() << std::endl;
			}
		}
		servantRegistrationMutex->unlock();

		return success;
	};

	template<typename T> bool registerServant(T* servant)
	{
		bool success = false;

		servantRegistrationMutex->lock();
		{
			try {
				poa->activate_object(servant);
				success = true;
			}
			catch(CORBA::SystemException& ex) {
				errStream << "Caught CORBA::" << ex._name() << std::endl;
			}
			catch(CORBA::Exception& ex) {
				errStream << "Caught CORBA::Exception: " << ex._name() << std::endl;
			}
			catch(omniORB::fatalException& fe) {
				errStream << "Caught omniORB::fatalException:" << std::endl;
				errStream << "  file: " << fe.file() << std::endl;
				errStream << "  line: " << fe.line() << std::endl;
				errStream << "  mesg: " << fe.errmsg() << std::endl;
			}
		}
		servantRegistrationMutex->unlock();

		return success;
	};

	template<typename T> bool unregisterServant(T* servant)
	{
		bool success = false;

		servantRegistrationMutex->lock();
		{
			try {
		//		poa->deactivate_object(servant);
				success = true;
			}
			catch(CORBA::SystemException& ex) {
				errStream << "Caught CORBA::" << ex._name() << std::endl;
			}
			catch(CORBA::Exception& ex) {
				errStream << "Caught CORBA::Exception: " << ex._name() << std::endl;
			}
			catch(omniORB::fatalException& fe) {
				errStream << "Caught omniORB::fatalException:" << std::endl;
				errStream << "  file: " << fe.file() << std::endl;
				errStream << "  line: " << fe.line() << std::endl;
				errStream << "  mesg: " << fe.errmsg() << std::endl;
			}
		}
		servantRegistrationMutex->unlock();

		return success;
	};



private:

	void tokenize(std::string inputString, std::string delimitor, 
		std::vector<std::string> &tokens) const;

	bool getRootContext(CosNaming::NamingContext_var & context);

	std::stringstream errStream;

	CORBA::ORB_var orb;
	PortableServer::POA_var poa;
	PortableServer::POAManager_var poa_manager;
	std::vector<CORBA::Object_var> servantRefs;

	int argc_l;
	char** argv_l;

	bool running;
	bool shutdown;

	omni_mutex* servantRegistrationMutex;

	omni_mutex* orbRunningMutex;
	omni_condition* orbRunningCondition;

};


#endif
