#ifndef STI_TIMINGENGINE_DOCUMENTATIONOPTIONS_H
#define STI_TIMINGENGINE_DOCUMENTATIONOPTIONS_H

namespace STI
{
namespace TimingEngine
{

class DocumentationOptions
{
public:
	virtual ~DocumentationOptions() {}

	virtual bool saveData() = 0;
	virtual std::string targetDirectory() = 0;
};

class LocalDocumentationOptions : public DocumentationOptions
{
public:
	LocalDocumentationOptions(bool save, std::string targetDir) : save_l(save), dir(targetDir) {}

	bool saveData() { return save_l; }
	std::string targetDirectory() { return dir; }

private:
	bool save_l;
	std::string dir;
};

}
}

#endif

