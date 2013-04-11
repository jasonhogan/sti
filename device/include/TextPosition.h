#ifndef STI_PARSE_TEXTPOSITION_H
#define STI_PARSE_TEXTPOSITION_H


namespace STI
{
namespace Parse
{

class TextPosition
{
public:
	const std::string& file() const;
	long line() const;
};


}
}

#endif

