
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <types.h>
#include <utils.h>
#include <stdlib.h>
#include <ctype.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <time.h>

using namespace std;

//Different compilers read numeric constants differently
#ifdef __GNUC__
#  define ULL10TO8  100000000ULL
#  define ULL10TO16 10000000000000000ULL
#else
#  define ULL10TO8  100000000
#  define ULL10TO16 10000000000000000
#endif

//const uInt32 Max32bit = 42*ULL10TO8+94967295;
//const uInt64 Max64bit = 1844*ULL10TO16+67440737*ULL10TO8+9551616;



namespace STI
{
namespace Utils
{

bool compareTValMixed(const STI::Types::TValMixed& left, const STI::Types::TValMixed& right)
{
	if(left._d() != right._d()) return false;

	bool identical = true;

	switch(left._d())
	{
	case STI::Types::ValueNumber:
		identical = (left.number() == right.number());
		break;
	case STI::Types::ValueString:
		{
			std::string temp = left.stringVal();
			identical = (temp.compare( right.stringVal() ) == 0);
		}
		break;
	case STI::Types::ValueVector:
		identical = (left.vector().length() == right.vector().length());
		for(unsigned i = 0; identical && i < left.vector().length(); i++) {
			identical &= compareTValMixed(left.vector()[i], right.vector()[i]);
		}
		break;
	case STI::Types::ValueNone:
		identical = (left.emptyValue() == right.emptyValue());
		break;
	default:
		identical = false;
		break;
	}

	return identical;
}


std::string printTimeFormated(double time)
{
	double baseUnit_ns = 1.0;	//units of "time" in ns

	int  s = static_cast<int>(time / (1e9 / baseUnit_ns));
	int ms = static_cast<int>((time - s * (1e9 / baseUnit_ns))/ (1e6 / baseUnit_ns));
	int us = static_cast<int>((time - s * (1e9 / baseUnit_ns) - ms * (1e6 / baseUnit_ns)) / (1e3 / baseUnit_ns));
	int ns = static_cast<int>((time - s * (1e9 / baseUnit_ns) - ms * (1e6 / baseUnit_ns) - us * (1e3 / baseUnit_ns)) / (1e0 / baseUnit_ns));

	std::stringstream timeFormatted;

	//  <Time= 23s:53ms:101us:33ns, Channel=
	//  <Time= 23 s : 53 ms : 101 us : 33 ns, Channel=
	//  <Time=23s|53ms|101us|33ns, Channel=	
	//  <Time=23s | 53ms | 101us | 33ns, Channel=
	//  <Time=23 s:53 ms:101 us:33 ns, Channel=
	//  <Time=23 s, 53 ms, 101 us, 33 ns, Channel=
	//  <Time=23s, 53ms, 101us, 33ns, Channel=

	if(s > 0) { timeFormatted << s << "s|"; }
	if(s > 0 || ms > 0) { timeFormatted << ms << "ms|"; }
	if(s > 0 || ms > 0 || us > 0) { timeFormatted << us << "us|"; }
	if(s > 0 || ms > 0 || us > 0 || ns >= 0) { timeFormatted << ns << "ns"; }

	return timeFormatted.str();
}

bool fileExists(std::string filename)
{
	return fs::exists( fs::path(filename) );
}

	
std::string convertPathToURL(std::string path)
{
	std::string URLslash = "/";

	if (getNativePathSeparator().compare(URLslash) == 0)
		return path;

	std::string::size_type slashLoc = path.find( getNativePathSeparator() );
	
	while(slashLoc != std::string::npos)
	{
		path.replace(slashLoc, 1, URLslash);

		slashLoc = path.find( getNativePathSeparator(), slashLoc + 1 );
	}

	return path;
}

std::string getDirectory(std::string fullPath)
{
	//get native path separator
	std::string nativePathSep = getNativePathSeparator();
	
	std::string::size_type slash = fullPath.find_last_of(nativePathSep);

	if(slash != std::string::npos && slash > 0)
		return fullPath.substr(0, slash);
	else
		return "";
}

std::string getFilenameNoDirectory(std::string fullPath)
{
	//get native path separator
	std::string nativePathSep = getNativePathSeparator();
	
	std::string::size_type slash = fullPath.find_last_of(nativePathSep);

	if(slash != std::string::npos && slash >= 0 && ((slash + 1) < fullPath.length()))
		return fullPath.substr(slash + 1);
	else
		return fullPath;
}

std::string getFilenameNoExtension(std::string filename)
{
	//assumes that the only period "." in the filename is at the start of the extension

	std::string::size_type period = filename.find_last_of(".");

	if(period != std::string::npos && period > 0)
		return filename.substr(0, period);
	else
		return filename;
}
std::string getUniqueFilename(std::string baseFilename, std::string extension, fs::path& directory)
{
	stringstream base;
	fs::path candidate;

	unsigned i = 0;

	do {
		base << baseFilename;

		if(i == 0)
			base << extension;
		else
			base << "_" << i << extension;

		candidate.clear();
		candidate = base.str();
		base.str("");
		i++;
		
	} while(fs::exists(directory / candidate));

	return candidate.string();

}

std::string getRelativePath(std::string absPath, std::string absBasePath)
{
	return getRelativePath( fs::path(absPath), fs::path(absBasePath) );
}
//
//std::string getRelativePath(const fs::path& absPath, const fs::path& absBasePath)
//{
//	fs::path relativePath;
//
//	std::string separator = getNativePathSeparator();
//
//	std::string absPath_s = absPath.string();
//	std::string absBasePath_s = absBasePath.string();
//
//	std::size_t baseDirLoc = absPath_s.find( absBasePath_s );
//
//	std::string dtdRelDir;
//	if(baseDirLoc >= 0 && baseDirLoc != std::string::npos)
//	{
//		std::size_t sepPos = absBasePath_s.find(separator, 0);
//		while(sepPos < std::string::npos)
//		{
//			relativePath /= "../";
//			sepPos = absBasePath_s.find(separator, sepPos+1);
//		}
//		relativePath /= absPath_s.substr(baseDirLoc + absBasePath_s.length());
//	}
//	else
//	{
//		relativePath = absPath_s;
//	}
//
//	return relativePath.string();
//}

std::string getRelativePath(const fs::path& absSourcePath, const fs::path& absReferencePath)
{
	std::vector<std::string> sourceDirs;
	std::vector<std::string> referenceDirs;
	
	splitString(absSourcePath.string(), getNativePathSeparator(), sourceDirs);
	splitString(absReferencePath.string(), getNativePathSeparator(), referenceDirs);


	if(referenceDirs.size() > 0 && referenceDirs.back().compare("") != 0)
	{
		referenceDirs.push_back("");
	}

	unsigned i;
	for(i = 0; i < referenceDirs.size() && i < sourceDirs.size(); i++)
	{
		if(referenceDirs.at(i).compare(sourceDirs.at(i)) != 0)
			break;
	}

	fs::path relative;
	
	int j;
	for(j = 0; j < (static_cast<int>(referenceDirs.size() - i - 1)); j++)
		relative /= "..";
	
	for(j = i; j < static_cast<int>(sourceDirs.size()); j++)
		relative /= sourceDirs.at(j);

	return relative.string();
}

std::string getNativePathSeparator()
{
	fs::path pathSeparator("/");
	return pathSeparator.string();
}

void convertArgs(int argc, char** argvInput, std::vector<std::string>& argvOutput)
{
	for(int i=0; i < argc; i++)
		argvOutput.push_back( std::string( argvInput[i] ) );
}

void splitString(std::string inString, std::string delimiter, std::vector<std::string>& outVector)
{
	std::string::size_type tBegin = 0;
	std::string::size_type tEnd = 0;

	// splits the sting at every delimiter
	while(tEnd != string::npos)
	{
		tBegin = inString.find_first_not_of(delimiter, tEnd);
		tEnd = inString.find_first_of(delimiter, tBegin);
		
		if(tBegin != string::npos)
			outVector.push_back(inString.substr(tBegin, tEnd - tBegin));
		else
			outVector.push_back("");
	}
}

bool isUniqueString(std::string value, std::vector<std::string>& list)
{
	bool found = false;

	for(unsigned i = 0; i < list.size(); i++)
	{
		found |= ( list.at(i).compare( value ) == 0 );
	}
	return !found;
}



std::string getDateAndTime()
{
	time_t rawtime;
	time(&rawtime);

	return asctime( localtime(&rawtime) );
}

std::string generateUniqueTimeBasedFileName(tm* timeStruct, std::string extension, fs::path& directory)
{
	std::stringstream fileName;

	fileName << (timeStruct->tm_mon + 1) << "_" << (timeStruct->tm_mday) << "_" << (1900 + timeStruct->tm_year) 
		<< "-" 
		<< timeStruct->tm_hour << "_" << timeStruct->tm_min << "_" << timeStruct->tm_sec;
	
	return STI::Utils::getUniqueFilename(fileName.str(), extension, directory);

}



std::string replaceChar(std::string input, std::string removedChar, std::string replacementChar)
{
	string output = input;
	string::size_type loc;

	loc = output.find(removedChar, 0);

	while(loc != string::npos) 
	{
		output.replace(loc, 1, replacementChar);
		loc = output.find(removedChar, loc + 1);
	}

	return output;
}

}// Utils
}// STI

/*

string tolower(const string &a)
{
	string res(a);
	for(string::iterator i=res.begin(); i!=res.end(); i++)
		*i=tolower(*i);
	return res;
}

uInt32 str_to_u32(const string &str)
//returns an int from a string
{
	istringstream i(str);
	uInt32 x;
	i >> x;
	return x;
}

uInt64 str_to_u64(const string &str)
// returns an Int64 from a string - doesn't work for strings longer than 20 digits
{
	uInt64 tail=0;
	uInt64 middle=0;
	uInt64 head=0;

	if(str.length()<=8)
		tail = str_to_u32(str.substr(0,str.length()));

	if((str.length() <= 16) & (str.length()>8))
	{
		tail = str_to_u32(str.substr(str.length() - 8,8));
		middle = str_to_u32(str.substr(0,str.length() - 8));
	}
		
	if((str.length()>16) & (str.length()<= 20))
	{
		tail = str_to_u32(str.substr(str.length() - 8,8));
		middle = str_to_u32(str.substr(str.length() - 16,8));
		head = str_to_u32(str.substr(0,str.length()-16));
	}

	if(str.length()>20)
	{
		cerr << "Error: String is longer than an Int64 can hold." << endl;
		return 0;
	}

	uInt64 num = tail + (middle * ULL10TO8) + (head * ULL10TO16);
	return num;

}

string u32_to_str(uInt32 num,int width)
{
	ostringstream o;
	o.fill('0');
	o.width(width);
	o << num;
	return o.str();
}

string u16_to_str(uInt16 num)
{
	ostringstream o;
	o << num;
	return o.str();
}

string u64_to_str(uInt64 num)
{
	string res;
	uInt32  over;
	int    width=0;

	if(num>=ULL10TO16)
	{
		over = num/ULL10TO16;
		num %= ULL10TO16;
		res += u32_to_str(over,width);
		width=8;
	}

	if(num>=ULL10TO8)
	{
		over = num/ULL10TO8;
		num %= ULL10TO8;
		res += u32_to_str(over,width);
		width=8;
	}

	res += u32_to_str(num,width);

	return res;
}

double str_to_dbl(const std::string &str)
{
	istringstream i(str);
	double x;
	i >> x;
	return x;
}

std::string dbl_to_str(double num)
{
	ostringstream o;
	o << num;
	return o.str();
}

uInt16 dbl_to_range16(double in, double in_min, double in_max, uInt16 out_min,
	uInt16 out_max)
{
	if(in<in_min)
		return out_min;
	if(in>in_max)
		return out_max;
	return static_cast<uInt16>((out_max-out_min)*(in-in_min)/(in_max-in_min)+out_min);
}

uInt32 dbl_to_range32(double in, double in_min, double in_max, uInt32 out_min,
	uInt32 out_max)
{
	if(in<in_min)
		return out_min;
	if(in>in_max)
		return out_max;
	return static_cast<uInt32>((out_max-out_min)*(in-in_min)/(in_max-in_min)+out_min);
}

string u8_to_hex(uInt8 num)
{
	string res;
	uInt8   nibble;

	nibble=(num&0xf0)>>4;
	if(nibble>=10)
		res='a'+nibble-10;
	else
		res='0'+nibble;

	nibble=num&0x0f;
	if(nibble>=10)
		res+='a'+nibble-10;
	else
		res+='0'+nibble;

	return res;
}

string u16_to_hex(uInt16 num)
{
	string res;

	res+=u8_to_hex((num&0xff00)>>8);
	res+=u8_to_hex(num&0x00ff);

	return res;
}

string u32_to_hex(uInt32 num)
{
	string res;

	res=u8_to_hex((num&0xff000000)>>24);
	res+=u8_to_hex((num&0x00ff0000)>>16);
	res+=u8_to_hex((num&0x0000ff00)>>8);
	res+=u8_to_hex(num&0x000000ff);

	return res;
}

string u64_to_hex(Int64 num)
{
	string res;

	res=u32_to_hex(num>>32);
	res+=u32_to_hex(num&Max32bit);

	return res;
}

//This converts the first two characters of str into a number, assuming
//a hexadecimal format. If the string is shorter or the characters are
//not hexadecimal, return either zero or the vlaue of the first character
//only (if the second character is not hex).
uInt8 hex_to_u8(const string &str)
{
	uInt8 res=0;
	char c;

	if(str.length()>=1) {
		c=tolower(str[0]);
		if(c>='0' && c <='9')
			res=static_cast<uInt8>(c-'0');
		else if(c>='a' && c <='f')
			res=static_cast<uInt8>(c-'a')+10;
		else
			return 0;
	}

	if(str.length()>=2) {
		c=tolower(str[1]);
		if(c>='0' && c <='9')
			res=16*res+static_cast<uInt8>(c-'0');
		else if(c>='a' && c <='f')
			res=16*res+static_cast<uInt8>(c-'a')+10;
		else
			return res;
	}

	return res;
}

//Creates the checksum of str.
//Here, the first two characters are dropped (they should be "Sx", x={0, 1, 2, 3, 4, 5...})
//Then all further entries are taken as hexadecimal values, summed byte-wise
//Finally, ~sum is returned as a 2-character hex value
string SREC_chksum(const string &str)
{
	uInt8 sum=0;

	for(unsigned i=2; i<str.length(); i+=2)
		sum+=hex_to_u8(str.substr(i,2));

	return u8_to_hex(~sum);
}

*/


