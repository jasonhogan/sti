#ifndef PTSBCDCONVERTER_H
#define PTSBCDCONVERTER_H

#include <iostream>
#include <math.h>

#include <sstream>
#include <vector>
#include <bitset>
using namespace std;


class PTSBCDConverter
{
public:
	PTSBCDConverter() {};

	double getSpinCoreDecimal()
	{
		// LSB ... MSB
		//int dec1GHz   = (PTSPin(13) << 0) | (PTSPin(14) << 1) | (PTSPin(11) << 2);
		//int dec100MHz = (PTSPin(43) << 0) | (PTSPin(44) << 1) | (PTSPin(49) << 2) | (PTSPin(38) << 3);
		//int dec10MHz  = (PTSPin(15) << 0) | (PTSPin(16) << 1) | (PTSPin(40) << 2) | (PTSPin(41) << 3);
		//int dec1MHz   = (PTSPin(17) << 0) | (PTSPin(18) << 1) | (PTSPin(19) << 2) | (PTSPin(20) << 3);
		//int dec100KHz = (PTSPin( 1) << 0) | (PTSPin( 2) << 1) | (PTSPin(26) << 2) | (PTSPin(27) << 3);
		//int dec10KHz  = (PTSPin( 3) << 0) | (PTSPin( 4) << 1) | (PTSPin(28) << 2) | (PTSPin(29) << 3);
		//int dec1KHz   = (PTSPin( 5) << 0) | (PTSPin( 6) << 1) | (PTSPin(30) << 2) | (PTSPin(31) << 3);
		//int dec100Hz  = (PTSPin( 7) << 0) | (PTSPin( 8) << 1) | (PTSPin(32) << 2) | (PTSPin(33) << 3);
		//int dec10Hz   = (PTSPin( 9) << 0) | (PTSPin(34) << 1) | (PTSPin(10) << 2) | (PTSPin(35) << 3);
		//int dec1Hz    = (PTSPin(45) << 0) | (PTSPin(12) << 1) | (PTSPin(36) << 2) | (PTSPin(37) << 3);


		int dec1GHz   = (PTSPin(43) << 0) | (PTSPin(44) << 1) | (PTSPin(45) << 2);
		int dec100MHz = (PTSPin(15) << 0) | (PTSPin(16) << 1) | (PTSPin(40) << 2) | (PTSPin(41) << 3);
		int dec10MHz  = (PTSPin(17) << 0) | (PTSPin(18) << 1) | (PTSPin(19) << 2) | (PTSPin(20) << 3);
		int dec1MHz   = (PTSPin( 1) << 0) | (PTSPin( 2) << 1) | (PTSPin(26) << 2) | (PTSPin(27) << 3);
		int dec100KHz = (PTSPin( 3) << 0) | (PTSPin( 4) << 1) | (PTSPin(28) << 2) | (PTSPin(29) << 3);
		int dec10KHz  = (PTSPin( 5) << 0) | (PTSPin( 6) << 1) | (PTSPin(30) << 2) | (PTSPin(31) << 3);
		int dec1KHz   = (PTSPin( 7) << 0) | (PTSPin( 8) << 1) | (PTSPin(32) << 2) | (PTSPin(33) << 3);
		int dec100Hz  = (PTSPin( 9) << 0) | (PTSPin(10) << 1) | (PTSPin(34) << 2) | (PTSPin(35) << 3);
		int dec10Hz   = (PTSPin(11) << 0) | (PTSPin(12) << 1) | (PTSPin(36) << 2) | (PTSPin(37) << 3);
		int dec1Hz    = (PTSPin(13) << 0) | (PTSPin(14) << 1) | (PTSPin(38) << 2) | (PTSPin(39) << 3);



//		cout << "Dec Value from bits: " << dec100MHz << endl;

		return
			(
			pow(10.0, 9)*static_cast<double>(dec1GHz) +
			pow(10.0, 8)*static_cast<double>(dec100MHz) +
			pow(10.0, 7)*static_cast<double>(dec10MHz) +
			pow(10.0, 6)*static_cast<double>(dec1MHz) +
			pow(10.0, 5)*static_cast<double>(dec100KHz) +
			pow(10.0, 4)*static_cast<double>(dec10KHz) +
			pow(10.0, 3)*static_cast<double>(dec1KHz) +
			pow(10.0, 2)*static_cast<double>(dec100Hz) +
			pow(10.0, 1)*static_cast<double>(dec10Hz) +
			pow(10.0, 0)*static_cast<double>(dec1Hz)
			);
	}


	void setFrequency(double freq)
	{
		inputDigits.clear();
		ptsPins.reset();

		for(int i = 0; i <= 9; i++) {
			inputDigits.push_back( getDecimalDigit(freq, i) );	//inputDigits.at(i) is 10^(i) decimal place
		}

		convertToPTSBits();
		getSpinCoreDecimal();
	}


private:

	//digitNum = 0 is the least significant decimal value (1s place, 10^0)
	long getDecimalDigit(double value, unsigned digitNum)
	{
		stringstream buffer;

		buffer << static_cast<unsigned long long>(value);

		long digit = 0;

		int digitPos = buffer.str().length() - digitNum - 1;
		
		if( digitPos >= 0) {
			istringstream convert(buffer.str().substr(digitPos, 1));
			convert >> digit;
		}

		return digit;

	}
	void convertToPTSBits()
	{
		unsigned decade;

		//1 GHz
		decade = 9;
		setPTSPin(13, getBit(inputDigits.at(decade), 1));
		setPTSPin(14, getBit(inputDigits.at(decade), 2));
		setPTSPin(11, getBit(inputDigits.at(decade), 3));
		
		//100 MHz
		decade = 8;
		setPTSPin(43, getBit(inputDigits.at(decade), 1));
		setPTSPin(44, getBit(inputDigits.at(decade), 2));
		setPTSPin(49, getBit(inputDigits.at(decade), 3));
		setPTSPin(38, getBit(inputDigits.at(decade), 4));

		//10 MHz
		decade = 7;
		setPTSPin(15, getBit(inputDigits.at(decade), 1));
		setPTSPin(16, getBit(inputDigits.at(decade), 2));
		setPTSPin(40, getBit(inputDigits.at(decade), 3));
		setPTSPin(41, getBit(inputDigits.at(decade), 4));

		//1 MHz
		decade = 6;
		setPTSPin(17, getBit(inputDigits.at(decade), 1));
		setPTSPin(18, getBit(inputDigits.at(decade), 2));
		setPTSPin(19, getBit(inputDigits.at(decade), 3));
		setPTSPin(20, getBit(inputDigits.at(decade), 4));

		//100 KHz
		decade = 5;
		setPTSPin(1, getBit(inputDigits.at(decade), 1));
		setPTSPin(2, getBit(inputDigits.at(decade), 2));
		setPTSPin(26, getBit(inputDigits.at(decade), 3));
		setPTSPin(27, getBit(inputDigits.at(decade), 4));

		//10 KHz
		decade = 4;
		setPTSPin(3, getBit(inputDigits.at(decade), 1));
		setPTSPin(4, getBit(inputDigits.at(decade), 2));
		setPTSPin(28, getBit(inputDigits.at(decade), 3));
		setPTSPin(29, getBit(inputDigits.at(decade), 4));

		//1 KHz
		decade = 3;
		setPTSPin(5, getBit(inputDigits.at(decade), 1));
		setPTSPin(6, getBit(inputDigits.at(decade), 2));
		setPTSPin(30, getBit(inputDigits.at(decade), 3));
		setPTSPin(31, getBit(inputDigits.at(decade), 4));

		//100 Hz
		decade = 2;
		setPTSPin(7, getBit(inputDigits.at(decade), 1));
		setPTSPin(8, getBit(inputDigits.at(decade), 2));
		setPTSPin(32, getBit(inputDigits.at(decade), 3));
		setPTSPin(33, getBit(inputDigits.at(decade), 4));

		//10 Hz
		decade = 1;
		setPTSPin(9, getBit(inputDigits.at(decade), 1));
		setPTSPin(34, getBit(inputDigits.at(decade), 2));
		setPTSPin(10, getBit(inputDigits.at(decade), 3));
		setPTSPin(35, getBit(inputDigits.at(decade), 4));

		//1 Hz
		decade = 0;
		setPTSPin(45, getBit(inputDigits.at(decade), 1));
		setPTSPin(12, getBit(inputDigits.at(decade), 2));
		setPTSPin(36, getBit(inputDigits.at(decade), 3));
		setPTSPin(37, getBit(inputDigits.at(decade), 4));

	}

	//void convertToPTSBits()
	//{
	//	unsigned decade;

	//	//1 GHz
	//	decade = 9;
	//	setPTSPin(43, getBit(inputDigits.at(decade), 1));
	//	setPTSPin(44, getBit(inputDigits.at(decade), 2));
	//	setPTSPin(45, getBit(inputDigits.at(decade), 3));
	//	
	//	//100 MHz
	//	decade = 8;
	//	setPTSPin(15, getBit(inputDigits.at(decade), 1));
	//	setPTSPin(16, getBit(inputDigits.at(decade), 2));
	//	setPTSPin(40, getBit(inputDigits.at(decade), 3));
	//	setPTSPin(41, getBit(inputDigits.at(decade), 4));

	//	//10 MHz
	//	decade = 7;
	//	setPTSPin(17, getBit(inputDigits.at(decade), 1));
	//	setPTSPin(18, getBit(inputDigits.at(decade), 2));
	//	setPTSPin(19, getBit(inputDigits.at(decade), 3));
	//	setPTSPin(20, getBit(inputDigits.at(decade), 4));

	//	//1 MHz
	//	decade = 6;
	//	setPTSPin(1, getBit(inputDigits.at(decade), 1));
	//	setPTSPin(2, getBit(inputDigits.at(decade), 2));
	//	setPTSPin(26, getBit(inputDigits.at(decade), 3));
	//	setPTSPin(27, getBit(inputDigits.at(decade), 4));

	//	//100 KHz
	//	decade = 5;
	//	setPTSPin(3, getBit(inputDigits.at(decade), 1));
	//	setPTSPin(4, getBit(inputDigits.at(decade), 2));
	//	setPTSPin(28, getBit(inputDigits.at(decade), 3));
	//	setPTSPin(29, getBit(inputDigits.at(decade), 4));

	//	//10 KHz
	//	decade = 4;
	//	setPTSPin(5, getBit(inputDigits.at(decade), 1));
	//	setPTSPin(6, getBit(inputDigits.at(decade), 2));
	//	setPTSPin(30, getBit(inputDigits.at(decade), 3));
	//	setPTSPin(31, getBit(inputDigits.at(decade), 4));

	//	//1 KHz
	//	decade = 3;
	//	setPTSPin(7, getBit(inputDigits.at(decade), 1));
	//	setPTSPin(8, getBit(inputDigits.at(decade), 2));
	//	setPTSPin(32, getBit(inputDigits.at(decade), 3));
	//	setPTSPin(33, getBit(inputDigits.at(decade), 4));

	//	//100 Hz
	//	decade = 2;
	//	setPTSPin(9, getBit(inputDigits.at(decade), 1));
	//	setPTSPin(10, getBit(inputDigits.at(decade), 2));
	//	setPTSPin(34, getBit(inputDigits.at(decade), 3));
	//	setPTSPin(35, getBit(inputDigits.at(decade), 4));

	//	//10 Hz
	//	decade = 1;
	//	setPTSPin(11, getBit(inputDigits.at(decade), 1));
	//	setPTSPin(12, getBit(inputDigits.at(decade), 2));
	//	setPTSPin(36, getBit(inputDigits.at(decade), 3));
	//	setPTSPin(37, getBit(inputDigits.at(decade), 4));

	//	//1 Hz
	//	decade = 0;
	//	setPTSPin(13, getBit(inputDigits.at(decade), 1));
	//	setPTSPin(14, getBit(inputDigits.at(decade), 2));
	//	setPTSPin(38, getBit(inputDigits.at(decade), 3));
	//	setPTSPin(39, getBit(inputDigits.at(decade), 4));

	//}

	bool getBit(unsigned dec, unsigned bitNum)
	{
		//Gets the bitNum bit from the decimal value dec
		//LSB corresponds to bitNum=1, MSB corresponds to bitNum=4

		return static_cast<bool> ( (dec >> (bitNum - 1)) & 0x1 );
	}

	void setPTSPin(unsigned pin, bool value)
	{
		ptsPins.set(pin-1, value);
	}
	bool PTSPin(unsigned pin)
	{
		return ptsPins.test(pin-1);
	}



	vector<unsigned> inputDigits;
	
	bitset<50> ptsPins;

};

//	long getDecimalDigitMod(double value, int digitNum)
//	{
//		unsigned long long tenToN = static_cast<unsigned long long>(pow(10.0, digitNum));
//		
////		cout << tenToN << ", " << (static_cast<long>(value) % tenToN) << endl;
//
//		long mod10n = static_cast<unsigned long long>(value) % (10*tenToN);
//
//		return static_cast<long>(mod10n / tenToN);
//	}
//
//	long getDecimalDigit(double value, unsigned digitNum)
//	{
//		double shiftedVal = value / pow(10.0, static_cast<double>(digitNum + 1));	//shift digitNumber to 0.1 place
//	
//		double temp = 10 * (shiftedVal - floor(shiftedVal));
//
//		double intpart = temp - floor(temp);
//
//		cout << endl << "Shifted:" << shiftedVal << "," << (shiftedVal - floor(shiftedVal)) << "," << 10 * (shiftedVal - floor(shiftedVal)) << endl;
//
//
//		
////		return static_cast<long>( 10 * (shiftedVal - floor(shiftedVal)) );
//		return 10 * (shiftedVal - floor(shiftedVal));
//	}








#endif
