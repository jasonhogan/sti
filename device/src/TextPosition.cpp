
#include "TextPosition.h"

#include <string>

using STI::TimingEngine::TextPosition;


TextPosition::TextPosition(const std::string& file, long line) 
: file_l(file), line_l(line)
{
}

const std::string& TextPosition::file() const
{
	return file_l;
}

long TextPosition::line() const
{
	return line_l;
}
