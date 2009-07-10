// ---------------------------------------------------------------------------
//  main
// ---------------------------------------------------------------------------

#include "XmlManager.h"

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

	xmlmanager2.PrintDocumentToScreen();

	
   return xmlmanager.errorCode;
}

