
/*! \file dummyserver.cpp
 *  \author Jason Hogan and Jon Harvey
 *  \brief A dummy server which registers ModeHandler and Parser.
 */

//#include "stdafx.h"


#include "Parser_i.h"
#include "ModeHandler_i.h"
#include <string>
#include <iostream>

using namespace std;

static CORBA::Boolean bindObjectToName(CORBA::ORB_ptr, CORBA::Object_ptr, const char*, const char*);
static void listen(void*);
static void talk(void*);

ModeHandler_i* modeHandlerServant;
Parser_i* parserServant;

int main(int argc, char **argv)
{
	try
	{    
		CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
 		CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
		PortableServer::POA_var poa	= PortableServer::POA::_narrow(poa_obj);
		PortableServer::POAManager_var poa_manager = poa->the_POAManager();


        parserServant = new Parser_i();
        modeHandlerServant = new ModeHandler_i();

        PortableServer::ObjectId_var parserID		= poa->activate_object(parserServant);
        PortableServer::ObjectId_var modeHandlerID	= poa->activate_object(modeHandlerServant);

        // Object registration - can be done with generic Object_var or specific type	
		STI_Client_Server::Parser_var parserObj = parserServant->_this();   
        CORBA::Object_var modeHandlerObj		= modeHandlerServant->_this();
		

        // I changed the bindObjectToName function as can be seen in these calls
        // -- Jon Harvey
        std::cout << "Binding servants to Nameservice\n\n\n" << endl;

        if( !bindObjectToName(orb, parserObj, "Parser", "Object") )
            return 1;
        if ( !bindObjectToName(orb, modeHandlerObj, "ModeHandler", "Object") )
            return 1;

		parserServant->add_ModeHandler(modeHandlerServant);      
		
		modeHandlerServant->_remove_ref();
        parserServant->_remove_ref();

                 	
		// threads
//		omni_thread::create(listen,(void*)"h1",omni_thread::PRIORITY_LOW);
//		omni_thread::create(talk,(void*)"h2",omni_thread::PRIORITY_LOW);

		poa_manager->activate();		
		orb->run();

	}
	catch(CORBA::SystemException& ex)
	{
		cerr << "Caught CORBA::" << ex._name() << endl;
	}
	catch(CORBA::Exception& ex)
	{
		cerr << "Caught CORBA::Exception: " << ex._name() << endl;
	}
	catch(omniORB::fatalException& fe)
	{
		cerr << "Caught omniORB::fatalException:" << endl;
		cerr << "  file: " << fe.file() << endl;
		cerr << "  line: " << fe.line() << endl;
		cerr << "  mesg: " << fe.errmsg() << endl;
	}
	
	return 0;
}


/**
 * bindObjectToName(CORBA::ORB_ptr, CORBA::Object_ptr, const char*, const char*)
 * 
 * Tries to bind objects to the CORBA name service. From the omniORB example code
 * with minor modifications by Jon Harvey and Jason Hogan.
 *
 * @param orb       The ORB to which the names will be bound
 * @param objref    The object which is being registered with the NameService
 * @param idstr     The ID string of the object's name
 * @param kindstr   The Kind string of the object's name
 */
static CORBA::Boolean
bindObjectToName(CORBA::ORB_ptr orb, CORBA::Object_ptr objref, 
        const char* idstr, const char* kindstr)
{
  CosNaming::NamingContext_var rootContext;

  try {
   // Obtain a reference to the root context of the Name service:
    CORBA::Object_var obj;
    obj = orb->resolve_initial_references("NameService");

    // Narrow the reference returned.
    rootContext = CosNaming::NamingContext::_narrow(obj);
 	

   if( CORBA::is_nil(rootContext) ) {
      cerr << "Failed to narrow the root naming context." << endl;
      return 0;
    }
  }
  catch (CORBA::NO_RESOURCES&) {
    cerr << "Caught NO_RESOURCES exception. You must configure omniORB "
	 << "with the location" << endl
	 << "of the naming service." << endl;
    return 0;
  }
  catch (CORBA::ORB::InvalidName&) {
    // This should not happen!
    cerr << "Service required is invalid [does not exist]." << endl;
    return 0;
  }


  try {
   // Bind a context called "STI_Client_Server" to the root context:

    CosNaming::Name contextName;
    contextName.length(1);
    contextName[0].id   = (const char*) "STI_Client_Server";       // string copied
    contextName[0].kind = (const char*) "Context"; // string copied
    // Note on kind: The kind field is used to indicate the type
    // of the object. This is to avoid conventions such as that used
    // by files (name.type -- e.g. test.ps = postscript etc.)

    CosNaming::NamingContext_var testContext;
    try {
      // Bind the context to root.
      testContext = rootContext->bind_new_context(contextName);
    }
    catch(CosNaming::NamingContext::AlreadyBound& ex) {
      // If the context already exists, this exception will be raised.
      // In this case, just resolve the name and assign testContext
      // to the object returned:
      CORBA::Object_var obj;
      obj = rootContext->resolve(contextName);
      testContext = CosNaming::NamingContext::_narrow(obj);
      if( CORBA::is_nil(testContext) ) {
        cerr << "Failed to narrow naming context." << endl;
        return 0;
      }
    }

    // Bind objref with name Echo to the testContext:
    CosNaming::Name objectName;
    //objectName.length(2);
    objectName.length(1);
    objectName[0].id   = (const char*) idstr;   // string copied
    objectName[0].kind = (const char*) kindstr; // string copied

    try {
     testContext->bind(objectName, objref);
     
    }
    catch(CosNaming::NamingContext::AlreadyBound& ex) {
     testContext->rebind(objectName, objref);
    }
    // Note: Using rebind() will overwrite any Object previously bound
    //       to /test/Echo with obj.
    //       Alternatively, bind() can be used, which will raise a
    //       CosNaming::NamingContext::AlreadyBound exception if the name
    //       supplied is already bound to an object.

    // Amendment: When using OrbixNames, it is necessary to first try bind
    // and then rebind, as rebind on it's own will throw a NotFoundexception if
    // the Name has not already been bound. [This is incorrect behaviour -
    // it should just bind].
  }
  catch(CORBA::TRANSIENT& ex) {
    cerr << "Caught system exception TRANSIENT -- unable to contact the "
         << "naming service." << endl
	 << "Make sure the naming server is running and that omniORB is "
	 << "configured correctly." << endl;

    return 0;
  }
  catch(CORBA::SystemException& ex) {
    cerr << "Caught a CORBA::" << ex._name()
	 << " while using the naming service." << endl;
    return 0;
  }
  return 1;
}

static void talk(void* arg)
{

	while (1)
	{
//		cin >> temp;
//		myecho->setServerMessage(temp);
//		myecho->serverSpoke(true);
	}
}


static void listen(void* arg)
{
			
	string temp;
	while (1) 
	{
//		temp = modeHandler->requesterName();

//		cout << temp;
//		if(myecho->clientSpoke())
//		{
//			cout << myecho->clientMessage() << endl;
//			myecho->clientSpoke(false);
//		}
	//	omni_thread::yield();
	//	myecho->echoString("test");
	//	omni_thread::sleep(2);
    }
}

