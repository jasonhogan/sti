
#ifndef UTILS_H
#define UTILS_H

// Filename: utils.h
//

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <types.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>


// Predefined constants
//extern const uInt32 Max32bit;
//extern const uInt64 Max64bit;

namespace STI
{
	namespace Utils
	{
		namespace fs = boost::filesystem;

		std::string getUniqueFilename(std::string baseFilename, std::string extension, fs::path& directory);

		std::string getRelativePath(std::string absPath, std::string absBasePath);
		std::string getNativePathSeparator();
		std::string getRelativePath(const fs::path& absSourcePath, const fs::path& absReferencePath);

		std::string getFilenameNoExtension(std::string filename);
		std::string getFilenameNoDirectory(std::string fullPath);
		std::string getDirectory(std::string fullPath);

		void convertArgs(int argc, char** argvInput, std::vector<std::string>& argvOutput);
		void splitString(std::string inString, std::string delimiter, std::vector<std::string>& outVector);
		bool isUniqueString(std::string value, std::vector<std::string>& list);

		template<typename T> bool stringToValue(std::string inString, T& outValue, std::ios::fmtflags numBase=std::ios::dec, std::streamsize precision=9)
		{
			//Returns true if the conversion is successful
			std::stringstream tempStream;
			tempStream.precision(precision);
			tempStream.setf( numBase, std::ios::basefield );

			tempStream << inString;
			tempStream >> outValue;

			return !tempStream.fail();
		}

		template<typename T> std::string valueToString(T inValue, std::string Default="", std::ios::fmtflags numBase=std::ios::dec, std::streamsize precision=9)
		{
			std::string outString;
			std::stringstream tempStream;
			tempStream.precision(precision);
			tempStream.setf( numBase, std::ios::basefield );

			tempStream << inValue;
			outString = tempStream.str();

			if( !tempStream.fail() )
				return outString;
			else
				return Default;
		}
	}
}


/*

// String tools
std::string tolower(const std::string &a);

// String to integer and back
uInt32 str_to_u32(const std::string &str);
uInt64 str_to_u64(const std::string &str);
std::string u16_to_str(uInt16 num);
std::string u32_to_str(uInt32 num,int width=0);
std::string u64_to_str(uInt64 num);

// String to double and back
double str_to_dbl(const std::string &str);
std::string dbl_to_str(double num);

//Fixed integer ranges (code handles out-of range inputs)
uInt16 dbl_to_range16(double in, double in_min, double in_max, uInt16 out_min,
	uInt16 out_max);
uInt32 dbl_to_range32(double in, double in_min, double in_max, uInt32 out_min,
	uInt32 out_max);

//Helpers for writing hexadecimal data
std::string u8_to_hex(uInt8 num);
std::string u16_to_hex(uInt16 num);
std::string u32_to_hex(uInt32 num);
std::string u64_to_hex(uInt64 num);
uInt8 hex_to_u8(const std::string &str);
std::string SREC_chksum(const std::string &str);
*/

#endif
