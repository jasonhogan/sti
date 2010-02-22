// Xml Manager Class



#include "XmlManager.h"

#include "xstring.h"

#include <string>
#include <vector>

XERCES_CPP_NAMESPACE_USE

//// ---------------------------------------------------------------------------
////  This is a simple class that lets us do easy (though not terribly efficient)
////  trancoding of char* data to XMLCh data.
//// ---------------------------------------------------------------------------
//class XStr
//{
//public :
//    // -----------------------------------------------------------------------
//    //  Constructors and Destructor
//    // -----------------------------------------------------------------------
//    XStr(const char* const toTranscode)
//    {
//        // Call the private transcoding method
//        fUnicodeForm = XMLString::transcode(toTranscode);
//    }
//
//    ~XStr()
//    {
//        XMLString::release(&fUnicodeForm);
//    }
//
//
//    // -----------------------------------------------------------------------
//    //  Getter methods
//    // -----------------------------------------------------------------------
//    const XMLCh* unicodeForm() const
//    {
//        return fUnicodeForm;
//    }
//
//private :
//    // -----------------------------------------------------------------------
//    //  Private data members
//    //
//    //  fUnicodeForm
//    //      This is the Unicode XMLCh format of the string.
//    // -----------------------------------------------------------------------
//    XMLCh*   fUnicodeForm;
//};
//
//#define X(str) XStr(str).unicodeForm()




//initialize Xerces using the constructor

//XERCES_CPP_NAMESPACE::DOMImplementation* XmlManager::documentImpl = NULL;
//XERCES_CPP_NAMESPACE::DOMImplementation* XmlManager::writerImpl = NULL;

XmlManager::XmlManager()
{
	documentImpl = NULL;
	writerImpl = NULL;
	doctype = NULL;
	doc = NULL;
	errReporter = NULL;
	theSerializer = NULL;
	parser = NULL;
	stdOutFormTarget = NULL;
	localFileFormTarget = NULL;
	theOutputDesc = NULL;
	errorCode = 0;

	// Initialize the XML4C2 system.
    try
    {
        XMLPlatformUtils::Initialize();
		errorCode = 0; //no error since we are able to load the XMLPlatform
    }
    catch(const XMLException& toCatch)
    {
        char *pMsg = XMLString::transcode( toCatch.getMessage() );

        XERCES_STD_QUALIFIER cerr 
			<< "Error during Xerces-c Initialization.\n"
			<< "  Exception message:"
            << pMsg;
        XMLString::release(&pMsg);
        errorCode = 1;
    }

	haveWriter             = false;
	haveDocument           = false;

	gSplitCdataSections    = true;
	gDiscardDefaultContent = true;

	gFormatPrettyPrint     = true;
	gWriteBOM              = false;

	gDoNamespaces          = false;
	gDoSchema              = false;
	gSchemaFullChecking    = false;
	gDoCreate              = false;

	//create a DOMParser
	parser = new XercesDOMParser();
	parser->setValidationScheme(XercesDOMParser::Val_Auto);
	parser->setDoNamespaces(gDoNamespaces);
	parser->setDoSchema(gDoSchema);
	parser->setValidationSchemaFullChecking(gSchemaFullChecking);
	parser->setCreateEntityReferenceNodes(gDoCreate);
	
	//create an error handler for the DOMParser
	errReporter = new DOMTreeErrorReporter();
	parser->setErrorHandler(errReporter);

	errorsOccured = false;
}


XmlManager::~XmlManager()
{
	//delete rootDOMNode;

	try
    {
       XMLPlatformUtils::Terminate();
    }
    catch( xercesc::XMLException& e )
    {
       char* message = xercesc::XMLString::transcode( e.getMessage() );

  	   std::cerr << "XML ttolkit teardown error: " << message << std::endl;
       XMLString::release( &message );
    }

//	if(doc != NULL)
//		doc->release();	//this also frees parser
//	
	//static variables
//	if(documentImpl != NULL)
//	{
//		delete documentImpl;
//		documentImpl = NULL;
//	}
//	if(writerImpl != NULL)
//	{
//		delete writerImpl;
//		writerImpl = NULL;
//	}
//	try {
//	if(doctype != NULL)
//	{
////		doctype->release();
////		doctype = NULL;
//	}
//	} catch(...) {
//	}


//	if(theOutputDesc != NULL)
//		theOutputDesc->release();
//	if(theSerializer != NULL)
//		theSerializer->release();

	if(stdOutFormTarget != NULL)
		delete stdOutFormTarget;
	if(localFileFormTarget != NULL)
		delete localFileFormTarget;
	if(errReporter != NULL)
		delete errReporter;
	   

}

std::string XmlManager::getElementByName(std::string name)
{

//	xstring("Core").toXMLCh()
//	doc->getElementsByTagName( XMLString::transcode("room") );

	const XMLCh* temp = XMLString::transcode("Core");
	//const XMLCh* temp2 = doc->getElementsByTagName( XMLString::transcode("room") )->item(0)->getAttributes();

	return std::string(
		xercesc::XMLString::transcode( 
		
		temp
		
		)
		);
}


int XmlManager::getXMLPlatformErrorCode()
{
	return errorCode;
}

void XmlManager::createDocument(std::string qualifiedName, std::string dtdFile, std::string rootElementName)
{
	documentImpl =  DOMImplementationRegistry::getDOMImplementation( xstring("Core").toXMLCh() );

	//This is the only supported implementation I know of: 
	//produces an XML document with <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
	//also can call this () with no implementation and set version, encoding and standalone later


	if (documentImpl != NULL && !haveDocument)
	{
		doctype = documentImpl->createDocumentType(
			xstring(qualifiedName).toXMLCh(),			// SYSTEM type
			0,										    // everyone sets this to null...
			xstring(dtdFile).toXMLCh()					// specify the DTD file

			);

		doc = documentImpl->createDocument(
			0,										// root element namespace URI.
			xstring(rootElementName).toXMLCh(),     // root element name
			doctype);								// document type object (DTD).

		haveDocument = true;

		rootDOMNode = new DOMNodeWrapper(doc->getDocumentElement(), doc);
	}

}
DOMNodeWrapper* XmlManager::getRootNode()
{
	return rootDOMNode;
}

void XmlManager::CreateDocument()
{
	createDocument("series", "series.dtd", "series");

	const XMLCh* temp = XMLString::transcode("Core");
	documentImpl =  DOMImplementationRegistry::getDOMImplementation(temp);
	//This is the only supported implementation I know of: 
	//produces an XML document with <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
	//also can call this () with no implementation and set version, encoding and standalone later

	if (documentImpl != NULL && !haveDocument)
	{
		doctype = documentImpl->createDocumentType(
			XMLString::transcode("series"),			// SYSTEM type
			0,										// everyone sets this to null...
			XMLString::transcode("series.dtd"));	//specify the DTD file

		doc = documentImpl->createDocument(
			0,										// root element namespace URI.
			XMLString::transcode("series"),         // root element name
			doctype);								// document type object (DTD).

		haveDocument = true;
		
		DOMElement* seriesElem = doc->getDocumentElement(); //creates a name for the root element
			   
			   //seriesElem->appendChild(styleSheet);

		DOMElement* titleElem = doc->createElement( xstring("title").toXMLCh() );
		seriesElem->appendChild( titleElem );
		titleElem->appendChild( doc->createTextNode( xstring("My Series Title").toXMLCh() ) );

			   DOMElement* dateElem = doc->createElement(xstring("date").toXMLCh());
			   seriesElem->appendChild(dateElem);

//			   DOMText* dateText = doc->createTextNode(xstring("7/10/09").toXMLCh());
//			   dateElem->appendChild(dateText);
			   
			   DOMElement* descriptionElem = doc->createElement(xstring("desc").toXMLCh());
			   seriesElem->appendChild(descriptionElem);
			   DOMText* descriptionText = doc->createTextNode(xstring("Launching atoms with different velocities.").toXMLCh());
			   descriptionElem->appendChild(descriptionText);

               DOMElement*  timingElem = doc->createElement(xstring("timing").toXMLCh());
	seriesElem->appendChild(timingElem);



	std::vector<DOMNode*> nodes;


	nodes.push_back( doc->getDocumentElement() );

//	DOMElement* root;
//	DOMElement* timingRoot, experimentsRoot;

/*
	DOMElement* root = doc->getDocumentElement();
	root->appendChild( doc->createElement( xstring("title").toXMLCh() ) )
		->appendChild( doc->createTextNode( xstring("My Series Title").toXMLCh() ) );

	DOMElement* timingRoot = root->appendChild( doc->createElement( xstring("timing").toXMLCh() ) );
	timingRoot->appendChild( doc->createElement( xstring("file").toXMLCh() ) )
			  ->appendChild( doc->createTextNode( xstring("timing.py").toXMLCh() ) );
	timingRoot->appendChild( doc->createElement( xstring("file").toXMLCh() ) )
			  ->appendChild( doc->createTextNode( xstring("channels.py").toXMLCh() ) );

	DOMElement* experimentsRoot = root->appendChild( doc->createElement( xstring("experiments").toXMLCh() ) );
	experimentsRoot->appendChild( doc->createElement( xstring("experiment").toXMLCh() ) )
						->appendChild( doc->createElement( xstring("file").toXMLCh() ) )
						->appendChild( doc->createTextNode( xstring("trial1.xml").toXMLCh() ) );
	experimentsRoot->appendChild( doc->createElement( xstring("experiment").toXMLCh() ) )
						->appendChild( doc->createElement( xstring("file").toXMLCh() ) )
						->appendChild( doc->createTextNode( xstring("trial2.xml").toXMLCh() ) );

*/
/*

	getRootNode()
		->appendChildElement("title")->appendTextNode("My Series Title")->endChild()
		->appendChildElement("date")->appendTextNode("7/10/09")->endChild()
		->appendChildElement("timing")
			->appendChildElement("file")->appendTextNode("trial1.xml")->endChild()
			->appendChildElement("file")->appendTextNode("trial2.xml")->endChild()
			->endChild()
		->appendChildElement("experiments");

*/



//	DOMDocumentFragment
/*
	root = getRootNode();
	root.appendChildElement("title").appendTextNode("My Series Title");
	root.appendChildElement("date").appendTextNode("7/10/09");



	class DOMNodeWrapper
	{
		DOMNodeWrapper* appendChildElement(string name);
		DOMNodeWrapper* appendTextNode(string text);
	


	};


	??
	int& f();
	int a = f();//calls copy constuctor

	int& b = f(); //assigns reference
*/

	}
	else
	{
		std::cerr << "A DOMDocument is already loaded. " 
			<< "Please use another instance of XmlManager for the new document." 
			<< std::endl;
	}
}
//
//void XmlManager::appendTextChild(DOMElement* parentElement, std::string elementName, std::string elementValue)
//DOMElement* titleElem = doc->createElement(X("title"));
//		seriesElem->appendChild( titleElem );
//		titleElem->appendChild( doc->createTextNode(X("My Series Title")) );

bool XmlManager::SetupWriter()
{
	XMLString::transcode("LS", tempStr, 99);

	writerImpl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	// get a serializer, an instance of DOMImplementationLS
	theSerializer = ( (DOMImplementationLS*)writerImpl )->createLSSerializer();
	
	theOutputDesc = ( (DOMImplementationLS*)writerImpl )->createLSOutput();
	
	DOMConfiguration* domConfiguration = theSerializer->getDomConfig();


	// set features if the serializer supports the feature/mode
	if (domConfiguration->canSetParameter(XMLUni::fgDOMWRTSplitCdataSections, gSplitCdataSections))
		domConfiguration->setParameter(XMLUni::fgDOMWRTSplitCdataSections, gSplitCdataSections);

	if (domConfiguration->canSetParameter(XMLUni::fgDOMWRTDiscardDefaultContent, gDiscardDefaultContent))
		domConfiguration->setParameter(XMLUni::fgDOMWRTDiscardDefaultContent, gDiscardDefaultContent);

	if (domConfiguration->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, gFormatPrettyPrint))
		domConfiguration->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, gFormatPrettyPrint);

	if (domConfiguration->canSetParameter(XMLUni::fgDOMWRTBOM, gWriteBOM))
		domConfiguration->setParameter(XMLUni::fgDOMWRTBOM, gWriteBOM);

	return true;
}

void XmlManager::PrintDocumentToScreen()
{
	if( haveDocument )
	{
		if( !haveWriter )
			haveWriter = XmlManager::SetupWriter();

		stdOutFormTarget = new StdOutFormatTarget();
		theOutputDesc->setByteStream(stdOutFormTarget);

		theSerializer->write(doc, theOutputDesc);

		// Clean up used memory
		if(stdOutFormTarget != NULL)
		{
			delete stdOutFormTarget;
			stdOutFormTarget = NULL;
		}
	}
	else {
		std::cerr << "No Document is loaded, so nothing can be printed." << std::endl;
	}
}

std::string XmlManager::getDocumentAsString()
{
	std::string result = "";

	if( haveDocument )
	{
		if( !haveWriter )
			haveWriter = XmlManager::SetupWriter();

		XMLCh* docString_Unicode = theSerializer->writeToString(doc);

		result = std::string( xercesc::XMLString::transcode( docString_Unicode ) );

		// release the memory

		// Clean up used memory
		if(docString_Unicode != NULL)
		{
			XMLString::release(&docString_Unicode); 
			docString_Unicode = NULL;
		}
	}
	else {
		std::cerr << "No Document is loaded, so nothing can be printed." << std::endl;
	}

	return result;
}

void XmlManager::PrintDocumentToFile(std::string filename)
{
	if(haveDocument)
	{
		if( !haveWriter )
			haveWriter = XmlManager::SetupWriter();

		localFileFormTarget = new LocalFileFormatTarget( filename.c_str() );
		theOutputDesc->setByteStream(localFileFormTarget);

		theSerializer->write(doc, theOutputDesc);		

		// Clean up used memory
		if(localFileFormTarget != NULL)
		{
			delete localFileFormTarget;
			localFileFormTarget = NULL;
		}
	}
	else 
	{
		std::cerr << "No Document is loaded, so nothing can be printed." << std::endl;
	}

}

void XmlManager::ImportDocument(std::string filename)
{
	//
	//  Parse the XML file, catching any XML exceptions that might propogate
	//  out of it.
	//
	
	if( !haveDocument )
	{
		try
		{
			parser->parse( filename.c_str() );
		}
		catch (const OutOfMemoryException&)
		{
			std::cerr << "OutOfMemoryException while parsing XML document '" 
				<< filename << "'." << std::endl;
			errorsOccured = true;
		}
		catch (const XMLException& e)
		{
			std::cerr << "An error occurred during parsing XML document '" 
				<< filename << "'." << std::endl
				<< "   Message: " << StrX(e.getMessage()) << std::endl;
			errorsOccured = true;
		}
		catch (const DOMException& e)
		{
			const unsigned int maxChars = 2047;
			XMLCh errText[maxChars + 1];

			std::cerr << "\n DOM Error during parsing: '" << filename << "'\n"
				<< "DOMException code is:  " << e.code << std::endl;

			if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
				std::cerr << "Message is: " << StrX(errText) << std::endl;

			errorsOccured = true;
		}
		catch (...)
		{
			std::cerr << "An unknown exception occurred while parsing XML document '" 
				<< filename << "'."
				<< std::endl;

			errorsOccured = true;
		}

		if ( !errorsOccured && !errReporter->getSawErrors() )
		{
			// get the DOM representation
			doc = parser->getDocument();
			haveDocument = true;
		}
	}
	else 
	{
		std::cerr << "A DOMDocument is already loaded in memory. " 
			<< std::endl 
			<< "Create a new instance of XmlManager to handle another document" 
			<< std::endl;
	}
}

