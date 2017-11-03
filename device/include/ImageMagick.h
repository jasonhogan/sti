#ifndef IMAGEMAGICK_H
#define IMAGEMAGICK_H

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <typeinfo>
#include <Magick++.h>
#include <sstream>
#include <time.h>

class ImageMagick
{

public:

	ImageMagick();
	~ImageMagick();

	class Metadatum {
		public:
			std::string tag;
			std::string value;
	};

	class MyImage 
	{
		public:

			MyImage():rotationAngle(0) {}
			~MyImage() {}

			std::vector <unsigned short> imageData;
			std::vector <Metadatum> metadata;

			std::string filename;
			std::string extension;

			int imageHeight;
			int imageWidth;

			//rotation angle (counter-clockwise)
			double rotationAngle;
	};

	std::vector <MyImage> imageVector;

	std::string fullfilename;

	class Metadata {
	public:
		std::vector <std::string> tags;
		std::vector <std::string> values;
	};

	std::vector <Metadata> metadata;

	void setMetadata(Magick::Image &imageData, MyImage &myImage);
	void clearMetadata(Magick::Image &imageData);

	bool saveToMultiPageGrey(std::vector <MyImage> &images);
//	bool saveToMultiMultiPageGrey(int numPerFile);
//	bool saveToMultipleGrey();
//	bool saveImageGrey();

//	bool readImageGrey();

	std::string intToString (int i);

	std::string makeTimeString();
	void writeImageVector();
	void writeImageVector(int numPerFile);

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