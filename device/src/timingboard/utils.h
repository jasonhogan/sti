#ifndef UTILS_H
#define UTILS_H

// Filename: utils.h
//

#include <string>
#include "types.h"

// String tools
std::string tolower(const std::string &a);

// String to integer and back
Int32 str_to_u32(const std::string &str);
Int64 str_to_u64(const std::string &str);
std::string u16_to_str(Int16 num);
std::string u32_to_str(Int32 num,int width=0);
std::string u64_to_str(Int64 num);

// String to double and back
double str_to_dbl(const std::string &str);
std::string dbl_to_str(double num);

//Fixed integer ranges (code handles out-of range inputs)
Int16 dbl_to_range16(double in, double in_min, double in_max, Int16 out_min,
	Int16 out_max);
Int32 dbl_to_range32(double in, double in_min, double in_max, Int32 out_min,
	Int32 out_max);

//Helpers for writing hexadecimal data
std::string u8_to_hex(Int8 num);
std::string u32_to_hex(Int32 num);
Int8 hex_to_u8(const std::string &str);
std::string SREC_chksum(const std::string &str);

#endif
