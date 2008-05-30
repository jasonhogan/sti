/*! \file ORBManager.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ORBManager
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

#include "ORBManager.h"

#include <omniORB4/omniURI.h>
#include <vector>
#include <sstream>
#include <string>

using std::vector;
using std::stringstream;
using std::string;
using std::endl;

ORBManager::ORBManager(int& argc, char** argv,
				  const char* orb_identifier,
				  const char* options[][2])
{	
	try {
		orb = CORBA::ORB_init(argc, argv, orb_identifier, options);
 	
		CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
		poa	= PortableServer::POA::_narrow(poa_obj);

		poa_manager = poa->the_POAManager();
	}
	catch(CORBA::SystemException& ex) {
		errStream << "Caught CORBA::" << ex._name() 
			<< " when trying to initialize ORBManager." << endl;
	}
	catch(CORBA::Exception& ex) {
		errStream << "Caught CORBA::Exception: " << ex._name() 
			<< " when trying to initialize ORBManager." << endl;
	}
	catch(omniORB::fatalException& fe) {
		errStream << "Caught omniORB::fatalException "
			<< "when trying to initialize ORBManager:" << endl;
		errStream << "  file: " << fe.file() << endl;
		errStream << "  line: " << fe.line() << endl;
		errStream << "  mesg: " << fe.errmsg() << endl;
	}
}

ORBManager::~ORBManager()
{
}

string ORBManager::errMsg()
{
	return errStream.str();
}

void ORBManager::run()
{
	poa_manager->activate();
	orb->run();
}

CORBA::Boolean ORBManager::bindObjectToName(CORBA::Object_ptr objref, 
											string objectStringName)
{
	CORBA::Object_var obj;
	CosNaming::NamingContext_var context;
	CosNaming::Name_var contextName;
	CosNaming::Name_var objectName;
	int i;
	vector<string> tokens;
	string::size_type tBegin; 
	string::size_type tEnd;

	// Splits the objectName into a vector of substrings
	// of the form {Context, Context, ..., Context, Object}
	while(tEnd != string::npos)
	{
		tBegin = objectStringName.find_first_not_of("/");
		tEnd = objectStringName.find_first_of("/", tBegin);

		tokens.push_back(objectStringName.substr(tBegin, tEnd - tBegin));
		objectStringName.erase(tBegin, tEnd - tBegin);
	}

	// Obtains the root context of the Name service
	try {
		CORBA::Object_var obj = orb->resolve_initial_references("NameService");
		
		// Narrow the reference returned.
		context = CosNaming::NamingContext::_narrow(obj);

		if( CORBA::is_nil(context) ) 
		{
			errStream << "Failed to narrow the root naming context." << endl;
			return false;
		}
	} 
	catch (CORBA::NO_RESOURCES&) {
		errStream << "Caught NO_RESOURCES exception. You must configure omniORB "
			<< "with the location" << endl << "of the naming service." << endl;
		return false;
	}
	catch (CORBA::ORB::InvalidName&) {
		// This should not happen!
		errStream << "Service required is invalid [does not exist]." << endl;
		return false;
	}

	// Binds all the contexts and then the Object
	try {
		// Sequentially binds a context with name tokens[i] to the previous context
		for(i = 0; i < tokens.size() - 1; i++)
		{
			contextName = omni::omniURI::stringToName(tokens[i].c_str());

			try {
				// Bind the context to the previous context
				context = context->bind_new_context(contextName);
			}
			catch(CosNaming::NamingContext::AlreadyBound& ex)
			{
				// If the context already exists, this exception will be raised.
				// In this case, just resolve the name and assign context
				// to the object
				errStream << "Warning: Caught CORBA::" << ex._name() << endl
					<< "when attemping to bind context '" << tokens[i] 
					<< "':" << endl << "The context already exists."  
					<< " Attempting to resolve the existing context." << endl;

				obj = context->resolve(contextName);
				context = CosNaming::NamingContext::_narrow(obj);
			
				if( CORBA::is_nil(context) ) 
				{
					errStream << "Failed to narrow naming context." << endl;
					return false;
				}
			}
		}

		// Now bind the object to the last context
		// The last token is the object name
		objectName = omni::omniURI::stringToName(tokens.back().c_str());
		
		try {
			context->bind(objectName, objref);
		}
		catch(CosNaming::NamingContext::AlreadyBound& ex)
		{
			// Using rebind() will overwrite any Object previously bound
			// to ../../context/objectName with objref.
			errStream << "Warning: Caught CORBA::" << ex._name() << endl
				<< "when attemping to bind Object '" << tokens.back() 
				<< "':" << endl << "The Object already exists "
				<< "in this context and will be overwritten." << endl;

			context->rebind(objectName, objref);
		}

	}
	catch(CORBA::TRANSIENT& ex)
	{
		errStream << "Caught system exception CORBA::" 
			<< ex._name() << " -- unable to contact the "
			<< "naming service." << endl
			<< "Make sure the naming server is running and that omniORB is "
			<< "configured correctly." << endl;
		
		return false;
	}
	catch(CORBA::SystemException& ex)
	{
		errStream << "Caught a CORBA::" << ex._name()
			<< " while using the naming service." << endl;
		return false;
	}

	return true;
}