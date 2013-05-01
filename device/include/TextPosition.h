#ifndef STI_TIMINGENGINE_TEXTPOSITION_H
#define STI_TIMINGENGINE_TEXTPOSITION_H

#include <string>

namespace STI
{
namespace TimingEngine
{

class TextPosition
{
public:
	TextPosition(std::string file, long line) 
		: _file(file), _line(line) {}
	const std::string& file() const;
	long line() const;
private:
	std::string _file;
	long _line;
};


}
}

#endif

