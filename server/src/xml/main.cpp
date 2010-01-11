// ---------------------------------------------------------------------------
//  main
// ---------------------------------------------------------------------------

#include "XmlManager.h"

using namespace std;

int main(int argC, char* /* argV[] */)
{

	XmlManager xmlmanager;
	xmlmanager.CreateDocument();
	xmlmanager.PrintDocumentToScreen();

	std::string test = "trial3.xml";
	xmlmanager.PrintDocumentToFile(test.c_str());

	XmlManager xmlmanager2;
	std::string test2 = "test.xml";
	xmlmanager2.ImportDocument(test2.c_str());
	std::cerr << "worked so far..." << std::endl;
	
//	xmlmanager2.PrintDocumentToScreen();
//	xmlmanager2.CreateDocument();

//	xmlmanager2.PrintDocumentToScreen();


	cerr << "Doc = " << endl << xmlmanager2.getDocumentAsString() << endl;

	cerr << "node: " << xercesc::XMLString::transcode( 
		xmlmanager2.doc->getChildNodes()->item(1)->getChildNodes()->item(2)->getNodeValue() ) << endl;



	return 0;
}

