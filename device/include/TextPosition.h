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
	TextPosition(const std::string& file, long line);

	const std::string& file() const;
	long line() const;

private:
	std::string file_l;
	long line_l;
};


}
}

#endif

