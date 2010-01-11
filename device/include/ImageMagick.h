#ifndef IMAGEMAGICK_H
#define IMAGEMAGICK_H

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <typeinfo>
#include <Magick++.h>

class ImageMagick
{

public:

	ImageMagick();
	~ImageMagick();

	
	int imageHeight;
	int imageWidth;
	std::string filename;

	std::vector <std::vector <unsigned short> > imageDataVector;
	std::vector <unsigned short> imageData;

	class Metadata {
	public:
		std::vector <std::string> tags;
		std::vector <std::string> values;
	};

	std::vector <Metadata> metadata;

	void addMetadata(Magick::Image &imageData, int i);

	bool saveToMultiPageGrey();
	bool saveToMultipleGrey();
	bool saveImageGrey();

	bool readImageGrey();


	template <typename T>
	bool convertVector(std::vector <std::vector <T> > &imageDataVectorT)
	{
		unsigned int i;
		bool error = false;

		if(imageDataVectorT.empty()){
			std::cerr << "Image vector empty." << std::endl;
			return true;
		}

		imageDataVector.clear();
		for (i = 0; i < imageDataVectorT.size() && !error; i++) {
			error = convert(imageDataVectorT.at(i));
			imageDataVector.push_back(imageData);
		}

		return false;
	};

	template <typename T>
	bool convert(std::vector <T> &imageDataT)
	{
		if(imageDataT.empty()){
			std::cerr << "Image vector empty." << std::endl;
			return true;
		}
		imageData.assign(imageDataT.begin(),imageDataT.end());
		return false;
	};

	template <typename T>
	bool saveToMultiPageGrey(std::vector <std::vector <T> > &imageDataVectorIn)
	{
		bool error = false;
		error = convertVector(imageDataVectorIn);
		if (!error) {
			error = saveToMultiPageGrey();
		}
		return error;
	};

	template <typename T>
	bool saveToMultipleGrey(std::vector <std::vector <T> > &imageDataVectorIn)
	{
		bool error = false;
		error = convertVector(imageDataVectorIn);
		if (!error) {
			error = saveToMultipleGrey();
		}
		return error;
	};

	template <typename T>
	bool saveImageGrey(std::vector <T> &imageDataIn)
	{
		bool error = false;
		error = convert(imageDataIn);
		if (!error) {
			error = saveImageGrey();
		}
		return error;
	};


private:


};

#endif