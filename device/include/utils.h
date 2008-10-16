
#ifndef UTILS_H
#define UTILS_H

// Filename: utils.h
//

#include <string>
#include <types.h>

// Predefined constants
extern const uInt32 Max32bit;
extern const uInt64 Max64bit;

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

#endif
