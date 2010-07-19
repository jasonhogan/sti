#ifndef IMAGEPREPROCESSOR_H
#define IMAGEPREPROCESSOR_H

#include <vector>
#include <string>
#include <ImageMagick.h>
#include "VectorArithmetic.h"

class ImagePreprocessor
{

public:

	ImagePreprocessor();
	~ImagePreprocessor();

	void processImages(std::vector <ImageMagick::MyImage> & imageVector);

private:

	void groupImageVector(std::vector <ImageMagick::MyImage> &imageVector, std::vector< std::vector <int> > groupedImagePosVec);
	
	void processAbsorptionImages(std::vector <ImageMagick::MyImage> &imageVector, std::vector< std::vector <int> > groupedImagePos);

	template<typename T> std::vector <unsigned short> toUShort(std::vector <T>& a)
	{
		std::vector <unsigned short> vOut;
		unsigned int i;

		for (i = 0; i < a.size(); i++)
		{
			vOut.push_back((unsigned short) a.at(i));
		}

		return vOut;
	}

	void boundDenominator(std::vector <double> & v);

};

#endif