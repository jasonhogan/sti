// Xml Manager Class



#include "XmlManager.h"

XERCES_CPP_NAMESPACE_USE

//initialize Xerces using the constructor

XmlManager::XmlManager()
{
	documentImpl = NULL;

	writerImpl = NULL;

	doctype = NULL;

	doc = NULL;

	errReporter = NULL;

	theSerializer = NULL;

	parser = NULL;

	myFormTarget = NULL;

	theOutputDesc= NULL; //added
	
	// Initialize the XML4C2 system.
    try
    {
        XMLPlatformUtils::Initialize();
    }

    catch(const XMLException& toCatch)
    {
        char *pMsg = XMLString::transcode(toCatch.getMessage());
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
             << "  Exception message:"
             << pMsg;
        XMLString::release(&pMsg);
        errorCode = 1;
    }
	errorCode = 0; //no error since we are able to load the XMLPlatform

	haveWriter = false;
	haveDocument = false;

	gSplitCdataSections    = true;
	gDiscardDefaultContent = true;

	gFormatPrettyPrint     = true;
	gWriteBOM              = false;

	gDoNamespaces          = false;
	gDoSchema              = false;
	gSchemaFullChecking    = false;
	gDoCreate              = false;

	//create a DOMParser
	parser = new XercesDOMParser;
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


	doc->release();

	XMLPlatformUtils::Terminate();

}
//Create a document
void XmlManager::CreateDocument()
	{
		const XMLCh* temp = XMLString::transcode("Core");

	

      documentImpl =  DOMImplementationRegistry::getDOMImplementation(temp);
	   // This is the only supported implementation I know of: 
	   //produces an XML document with <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
		// also can call this () with no implementation and set version, encoding and standalone later
	
//std::cerr << "working inside create document" << std::endl;

       if (documentImpl != NULL && haveDocument !=true)
       {
           
           
               doctype = documentImpl->createDocumentType(
							XMLString::transcode("series"),	// SYSTEM type
							0,				// everyone sets this to null...
							XMLString::transcode("series.dtd")); //specify the DTD file
			   
			   
			   doc = documentImpl->createDocument(
                           0,                    // root element namespace URI.
                           XMLString::transcode("series"),         // root element name
                           doctype);             // document type object (DTD).

				haveDocument = true;

			
	   }
	   else
	   {
		   std::cerr << "A DOMDocument is already loaded. Please use another instance of XmlManager for the new document." << std::endl;
	   }
}


bool XmlManager::SetupWriter()
{
	// get a serializer, an instance of DOMWriter
	
	XMLString::transcode("LS", tempStr, 99);
	writerImpl          = DOMImplementationRegistry::getDOMImplementation(tempStr);
	
	theSerializer = ((DOMImplementationLS*)writerImpl)->createLSSerializer();
	DOMConfiguration* domConfiguration = theSerializer->getDomConfig();
	theOutputDesc = ((DOMImplementationLS*)writerImpl)->createLSOutput();


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
	if(haveDocument == true)
	{
		if(haveWriter != true)
			haveWriter = XmlManager::SetupWriter();

		myFormTarget = new StdOutFormatTarget();

		theOutputDesc->setByteStream(myFormTarget);
		
	theSerializer->write(doc, theOutputDesc);
	
	//theSerializer->writeNode(myFormTarget, *doc);	//2.0

		// Clean up used memory
		if(myFormTarget != NULL) {
			delete myFormTarget;
			myFormTarget = NULL;
		}
	}
	else {
		std::cerr << "No Document is loaded, so nothing can be printed." << std::endl;
	}

}

void XmlManager::PrintDocumentToFile(const char* const filename)
{
	if(haveDocument == true)
	{
		if(haveWriter != true)
			haveWriter = XmlManager::SetupWriter();

		myFormTarget = new LocalFileFormatTarget(filename);
		
		theOutputDesc->setByteStream(myFormTarget);
		

		theSerializer->write(doc, theOutputDesc);		

//		theSerializer->writeNode(myFormTarget, *doc);

		// Clean up used memory
		if(myFormTarget != NULL) {
			delete myFormTarget;
			myFormTarget = NULL;
		}
	}
	else {
		std::cerr << "No Document is loaded, so nothing can be printed." << std::endl;
	}

}

void XmlManager::ImportDocument(const char* const filename)
{
	//
	//  Parse the XML file, catching any XML exceptions that might propogate
	//  out of it.
	//
	
	if(haveDocument != true)
	{
    try
    {
        parser->parse(filename);
    }
    catch (const OutOfMemoryException&)
    {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        errorsOccured = true;
    }
    catch (const XMLException& e)
    {
        XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n   Message: "
             << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        errorsOccured = true;
    }

    catch (const DOMException& e)
    {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];

        XERCES_STD_QUALIFIER cerr << "\n DOM Error during parsing: '" << filename << "'\n"
             << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

        if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
             XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;

        errorsOccured = true;
    }

    catch (...)
    {
        XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n " << XERCES_STD_QUALIFIER endl;
        errorsOccured = true;
    }

	if (!errorsOccured && !errReporter->getSawErrors())
		{
			// get the DOM representation
			doc = parser->getDocument();
			haveDocument = true;
		}
	}
	else {
	
		std::cerr << "A DOMDocument is already loaded in memory. Create a new instance of XmlManager to handle another document" << std::endl;

	}


}

