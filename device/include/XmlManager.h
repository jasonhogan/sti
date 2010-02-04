// Xml Manager class


#ifndef XMLMANAGER_H
#define XMLMANAGER_H

#include <string>
#include <vector>

// xerces parser includes
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

// error handling utilites
#include <xercesc/util/OutOfMemoryException.hpp>
#include "DOMTreeErrorReporter.hpp"
#include "DOMPrintErrorHandler.hpp"

// output to file / screen functions
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/Xerces_autoconf_config.hpp>

#include <DOMNodeWrapper.h>

class XmlManager {
public:

	XmlManager();
	~XmlManager();

	void CreateDocument();
	void createDocument(std::string qualifiedName, std::string dtdFile, std::string rootElementName);
	bool SetupWriter();
	void PrintDocumentToScreen();
	void PrintDocumentToFile(std::string filename);
	void ImportDocument(std::string filename);
	
	std::string getDocumentAsString();

	DOMNodeWrapper* getRootNode();

	int getXMLPlatformErrorCode();

	std::string getElementByName(std::string name);

	XERCES_CPP_NAMESPACE::DOMDocument* doc;

private:
	
	DOMNodeWrapper* rootDOMNode;

	XMLCh tempStr[100];
	int errorCode;
	bool errorsOccured;
	bool haveWriter;
	bool haveDocument;
	
	XERCES_CPP_NAMESPACE::DOMImplementation* documentImpl; //might have to be static 
	XERCES_CPP_NAMESPACE::DOMImplementation* writerImpl; //might have to be static
	
	XERCES_CPP_NAMESPACE::DOMDocumentType* doctype;
	XERCES_CPP_NAMESPACE::DOMLSSerializer* theSerializer;
	XERCES_CPP_NAMESPACE::XercesDOMParser* parser;
	XERCES_CPP_NAMESPACE::XMLFormatTarget* stdOutFormTarget;
	XERCES_CPP_NAMESPACE::XMLFormatTarget* localFileFormTarget;
	XERCES_CPP_NAMESPACE::DOMLSOutput*     theOutputDesc;

	DOMTreeErrorReporter *errReporter;

	// options defined for the parser and the parser output - move location??

	bool gSplitCdataSections;
	bool gDiscardDefaultContent;
	bool gFormatPrettyPrint;
	bool gWriteBOM;
	bool gDoNamespaces;
	bool gDoSchema;
	bool gSchemaFullChecking;
	bool gDoCreate;

};

#endif
