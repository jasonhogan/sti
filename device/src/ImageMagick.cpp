#include "ImageMagick.h"

ImageMagick::ImageMagick()
{
}


ImageMagick::~ImageMagick()
{
}

void ImageMagick::setMetadata(Magick::Image &image, MyImage &myImage)
{
	unsigned int j;
	std::string attr_str = "";

	for(j = 0; j < myImage.metadata.size(); j++)
	{
		attr_str += myImage.metadata.at(j).tag + ": " + myImage.metadata.at(j).value +"; ";
	}
	image.attribute("EXIF:ImageDescription",attr_str);
	image.comment(attr_str);
}

void ImageMagick::clearMetadata(Magick::Image &imageData)
{
	std::string attr_str = "";

	imageData.attribute("EXIF:ImageDescription",attr_str);
	imageData.comment(attr_str);
}

std::string ImageMagick::makeTimeString()
{
	std::string localTimeString;
	size_t found;

	struct tm localTime;
	__int64 rawTime;
	char time_buf[26];
	errno_t err;

	_time64( &rawTime );

	// Obtain coordinated universal time: 
	err = _localtime64_s( &localTime, &rawTime );
	if (err)
	{
		std::cerr << "Invalid Argument to _gmtime64_s." << std::endl;
	}

	// Convert to an ASCII representation 
	err = asctime_s(time_buf, 26, &localTime);
	if (err)
	{
		std::cerr << "Invalid Argument to asctime_s." << std::endl;
	}		

	localTimeString = time_buf;

	found=localTimeString.find_first_of(":");

	while (found!=std::string::npos)
	{
		localTimeString[found]='_';
		found=localTimeString.find_first_of(":",found+1);
	}	

	found=localTimeString.find_first_of("\n");

	while (found!=std::string::npos)
	{
		localTimeString.erase(found, 1);
		found=localTimeString.find_first_of("\n",found+1);
	}

	return localTimeString;
}

void ImageMagick::writeImageVector(int numPerFile)
{
	int i,j;
	std::string tempFilename;
	int numFiles = imageVector.size()/numPerFile;

	if(imageVector.empty()){
		std::cerr << "Image vector empty. Be sure to convertVector or convert image data" << std::endl;
		return;
	}

	for(j = 0; j < numFiles; j++)
	{
		for (i = 0; i < numPerFile; i++) {
			tempFilename = imageVector.at(j*numPerFile + i).filename;
			if(numFiles != 1){
				imageVector.at(j*numPerFile + i).filename = tempFilename + "_" + intToString(j);
			}
		}
	}

	writeImageVector();

}

void ImageMagick::writeImageVector()
{
	int i;
	std::string filename;
	std::string extension;
	std::vector <int> imagePos;
	std::vector <MyImage> tempImageVector;
	bool error = false;

	std::vector <MyImage>::iterator iter;

	if (imageVector.empty()){
		std::cerr << "ImageMagick: imageVector empty" << std::endl;
	}

	std::cerr << "Got Here too" << std::endl;
	while (!(imageVector.empty()) && !error)
	{
		filename = imageVector.front().filename;
		extension = imageVector.front().extension;

		std::cerr << "front filename" << std::endl;
		std::cerr << filename << std::endl;

		for(i = 0; i < (signed) imageVector.size(); i++)
		{
			std::cerr << "inner " << i << std::endl;
			std::cerr << imageVector.at(i).filename << std::endl;
			if (filename.compare(imageVector.at(i).filename) == 0 && extension.compare(imageVector.at(i).extension) == 0)
			{
				imagePos.push_back(i);
			}
		}

		for (i = 0; i < (signed) imagePos.size(); i++)
		{
			tempImageVector.push_back(imageVector.at(imagePos.at(i)));
		}
		
		error = saveToMultiPageGrey(tempImageVector);

		while (!(imagePos.empty()))
		{
			imageVector.erase(imageVector.begin()+imagePos.back());
			imagePos.pop_back();
		}
		tempImageVector.clear();
		
	}
}


bool ImageMagick::saveToMultiPageGrey(std::vector <MyImage> &images) 
{
	Magick::Image image;
	std::list <Magick::Image> imageList;
	const std::string magickMap("I");
	unsigned int i;
	unsigned short myShort = 0;

	//filenames are assumed to be the same for all images at this point
	std::string fullfilename = images.at(0).filename + images.at(0).extension;

	if(images.empty()){
		std::cerr << "Image vector empty. Be sure to convertVector or convert image data" << std::endl;
		return true;
	}

	try {
		for (i = 0; i < images.size(); i++) {
			
			image.read(images.at(i).imageWidth, images.at(i).imageHeight,  
				magickMap, MagickCore::ShortPixel, &(images.at(i).imageData)[0]);
			image.rotate(images.at(i).rotationAngle);
			setMetadata(image, images.at(i));
			
			imageList.push_back(image);
			//clearMetadata(image);
		}

		Magick::writeImages(imageList.begin(), imageList.end(), fullfilename, true);
	}
	catch ( std::exception &error_ ) {
		std::cerr << "Caught exception: " << error_.what() << std::endl;
		return true;
    }

	return false;
}
/*
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
		image.write(fullfilename);
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
*/
std::string ImageMagick::intToString (int i)
{
	std::stringstream strm;
	strm << i;
	return strm.str();
}