#include "ImageMagick.h"

ImageMagick::ImageMagick()
{
}


ImageMagick::~ImageMagick()
{
}

void ImageMagick::addMetadata(Magick::Image &imageData, int i)
{
	unsigned int j;
	std::string attr_str = "";

	for(j = 0; j < metadata.at(i).tags.size(); j++)
	{
		attr_str += metadata.at(i).tags.at(j) + ": " + metadata.at(i).values.at(j) +"; ";
	}
	imageData.attribute("EXIF:ImageDescription",attr_str);
	imageData.comment(attr_str);
}

void ImageMagick::clearMetadata(Magick::Image &imageData)
{
	std::string attr_str = "";

	imageData.attribute("EXIF:ImageDescription",attr_str);
	imageData.comment(attr_str);
}

bool ImageMagick::saveToMultiPageGrey() 
{
	Magick::Image image;
	std::list <Magick::Image> imageList;
	const std::string magickMap("I");
	unsigned int i;
	unsigned short myShort = 0;

	if(imageDataVector.empty()){
		std::cerr << "Image vector empty. Be sure to convertVector or convert image data" << std::endl;
		return true;
	}

	try {
		for (i = 0; i < imageDataVector.size(); i++) {
			image.read(imageWidth, imageHeight, magickMap, MagickCore::ShortPixel, &imageDataVector.at(i)[0]);
			addMetadata(image, i);
			imageList.push_back(image);
			//clearMetadata(image);
		}

		fullfilename = filepath + filename + extension;
		Magick::writeImages(imageList.begin(), imageList.end(), fullfilename, true);

	}
	catch ( std::exception &error_ ) {
		std::cerr << "Caught exception: " << error_.what() << std::endl;
		return true;
    }

	return false;
}

bool ImageMagick::saveToMultiMultiPageGrey(int numPerFile) 
{
	Magick::Image image;
	std::list <Magick::Image> imageList;
	const std::string magickMap("I");
	unsigned int i,j;
	unsigned short myShort = 0;
	std::string tempFilename;
	int numFiles = imageDataVector.size()/numPerFile;

	if(imageDataVector.empty()){
		std::cerr << "Image vector empty. Be sure to convertVector or convert image data" << std::endl;
		return true;
	}

	try {
		for(j = 0; j < numFiles; j++)
		{
			imageList.clear();
			for (i = 0; i < numPerFile; i++) {
				image.read(imageWidth, imageHeight, magickMap, MagickCore::ShortPixel, &imageDataVector.at(j+i)[0]);
				tempFilename = filename;
				if(numFiles != 1){
					tempFilename = tempFilename + "_" + intToString(j);
				}
				addMetadata(image, j*numPerFile + i);
				imageList.push_back(image);
				//clearMetadata(image);
			}

			fullfilename = filepath + tempFilename + extension;
			Magick::writeImages(imageList.begin(), imageList.end(), fullfilename, true);
		}

	}
	catch ( std::exception &error_ ) {
		std::cerr << "Caught exception: " << error_.what() << std::endl;
		return true;
    }

	return false;
}



bool ImageMagick::saveToMultipleGrey() 
{
	Magick::Image image;
	std::list <Magick::Image> imageList;
	const std::string magickMap("I");
	unsigned int i;
	int pos;
	std::string tempFilename = filepath + filename + extension;

	if(imageDataVector.empty()){
		std::cerr << "Image vector empty. Be sure to convertVector or convert image data" << std::endl;
		return true;
	}

	pos = filename.find_last_of('.');
	tempFilename.insert(pos, "_%02d");

	try {
		for (i = 0; i < imageDataVector.size(); i++) {
			image.read(imageWidth, imageHeight, magickMap, MagickCore::ShortPixel, &imageDataVector.at(i)[0]);
			imageList.push_back(image);
		}

		Magick::writeImages(imageList.begin(), imageList.end(), tempFilename, false);

	}
	catch ( std::exception &error_ ) {
		std::cerr << "Caught exception: " << error_.what() << std::endl;
		return true;
    }

	return false;
}


bool ImageMagick::saveImageGrey() 
{
	Magick::Image image;
	const std::string magickMap("I");
	fullfilename = filepath + filename + extension;

	if(imageData.empty()){
		std::cerr << "Image vector empty. Be sure to convertVector or convert image data" << std::endl;
		return true;
	}

	try {
		image.read(imageWidth, imageHeight, magickMap, MagickCore::ShortPixel, &imageData[0]);
		image.write(filename);
	}
	catch ( std::exception &error_ ) {
		std::cerr << "Caught exception: " << error_.what() << std::endl;
		return true;
    }

	return false;
}

bool ImageMagick::readImageGrey() 
{
	Magick::Image image;
	std::list <Magick::Image> imageList;
	const std::string magickMap("I");

	fullfilename = filepath + filename + extension;

	try {
		Magick::readImages(&imageList, filename);

		if (!imageList.empty()){
			imageDataVector.clear();
		}
		while(!imageList.empty()) {
			imageWidth = imageList.front().baseColumns();
			imageHeight = imageList.front().baseRows();
			imageData.assign(imageHeight*imageWidth, 0);
			imageList.front().write(0, 0, imageWidth, imageHeight, magickMap, MagickCore::ShortPixel, &imageData[0]);
			imageDataVector.push_back(imageData);
			imageList.pop_front();
		}
	}
	catch ( std::exception &error_ ) {
		std::cerr << "Caught exception: " << error_.what() << std::endl;
		return true;
    }

	return false;
}

std::string ImageMagick::intToString (int i)
{
	std::stringstream strm;
	strm << i;
	return strm.str();
}