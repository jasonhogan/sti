#ifndef STI_TIMINGENGINE_DOCUMENTATIONOPTIONS_H
#define STI_TIMINGENGINE_DOCUMENTATIONOPTIONS_H

namespace STI
{
namespace TimingEngine
{

class DocumentationOptions
{
public:

	virtual bool saveData() = 0;
};

class LocalDocumentationOptions : public DocumentationOptions
{
public:
	LocalDocumentationOptions(bool save) : save_l(save) {}

	bool saveData() { return save_l; }
private:
	bool save_l;
};

}
}

#endif

