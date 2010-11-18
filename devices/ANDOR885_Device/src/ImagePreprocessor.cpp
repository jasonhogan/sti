#include "ImagePreprocessor.h"

using namespace VectorArithmetic;

ImagePreprocessor::ImagePreprocessor()
{
}


ImagePreprocessor::~ImagePreprocessor()
{
}

void ImagePreprocessor::processImages(std::vector <ImageMagick::MyImage> & imageVector)
{
	std::vector <std::vector <int> > groupedImagePos;

	//group images by filename
	groupImageVector(imageVector, groupedImagePos);

	processAbsorptionImages(imageVector, groupedImagePos);
}

void ImagePreprocessor::groupImageVector(std::vector <ImageMagick::MyImage> &imageVector, std::vector< std::vector <int> > &groupedImagePosVec)
{
	unsigned int i;
	std::vector <std::string> filenames;
	std::string filename;
	std::vector <std::string> extensions;
	std::string extension;
	std::vector <int> imagePos;		//list of the position of the images in the original imageVector
	std::vector <int> erasePos;				//position in originalImagePos of 
	std::vector <int> groupedImagePos;
	bool error = false;

	std::vector <int>::iterator it;

	if (imageVector.empty()){
		std::cerr << "ImagePreprocessor: imageVector empty" << std::endl;
	}

	for (i = 0; i < imageVector.size(); i++)
	{
		filenames.push_back(imageVector.at(i).filename);
		extensions.push_back(imageVector.at(i).extension);
		imagePos.push_back(i);
	}

	while (!(imagePos.empty()) && !error)
	{
		filename = filenames.at(imagePos.at(0));
		extension = extensions.at(imagePos.at(0));

		for(it = imagePos.begin(), i = 0; it != imagePos.end(); it++, i++)
		{
			if (filename.compare(filenames.at(*it)) == 0 && extension.compare(extensions.at(*it)) == 0)
			{
				// Push back both the position in the imageVector and the position in the imagePos vector
				groupedImagePos.push_back(*it);
				erasePos.push_back(i);
			}
		}

		groupedImagePosVec.push_back(groupedImagePos);

		// Erase the positions already grouped
		while (!(erasePos.empty()))
		{
			imagePos.erase(imagePos.begin() + erasePos.back());
			erasePos.pop_back();
		}
		
		groupedImagePos.clear();
	}
}

void ImagePreprocessor::processAbsorptionImages(std::vector <ImageMagick::MyImage> &imageVector, std::vector< std::vector <int> > &groupedImagePos)
{
	std::vector <std::vector <int> >::iterator it;
	ImageMagick::MyImage *withCloud;
	ImageMagick::MyImage *withoutCloud;
	ImageMagick::MyImage *background;
	std::vector <double> numerator;
	std::vector <double> denominator;
	std::vector <double> absorptionData;
	std::vector <unsigned short> absorptionDataUS;
	ImageMagick::MyImage absorptionImage;
	double cameraSaturation = 16383;
	//double cameraSatFrac = (cameraSaturation - 100) / cameraSaturation;
	double imageNoise;
	double imageSatFrac;

	ImageMagick::Metadatum metadatum;

	bool error;

	for (it = groupedImagePos.begin(); it != groupedImagePos.end(); it++)
	{
		if (imageVector.at((*it).at(0)).filename.find("absorption") != std::string::npos && (*it).size() == 3)
		{
			withCloud =		&(imageVector.at((*it).at(0)));
			withoutCloud =	&(imageVector.at((*it).at(1)));
			background =	&(imageVector.at((*it).at(2)));

			numerator = minus(withoutCloud->imageData, withCloud->imageData, error);
			denominator = minus(withoutCloud->imageData, background->imageData, error);

			boundBelow(denominator, 1);

			imageNoise = rms(background->imageData, error);
			imageSatFrac = (cameraSaturation - imageNoise) / cameraSaturation;

			absorptionData = divide(numerator, denominator, error);
			boundBelow(absorptionData,0);
			//boundAbove(absorptionData,cameraSatFrac);
			boundAbove(absorptionData, imageSatFrac);
			absorptionData = times(absorptionData, cameraSaturation, error);

			absorptionDataUS = toUShort(absorptionData);

			absorptionImage.filename = withCloud->filename + " processed";
			absorptionImage.extension = withCloud->extension;
			absorptionImage.imageData = absorptionDataUS;
			absorptionImage.rotationAngle = withCloud->rotationAngle;
			absorptionImage.imageHeight = withCloud->imageHeight;
			absorptionImage.imageWidth = withCloud->imageWidth;
			absorptionImage.metadata = withCloud->metadata;

			metadatum.tag = "Image noise";
			metadatum.value = STI::Utils::valueToString(imageNoise);
			absorptionImage.metadata.push_back(metadatum);

			imageVector.push_back(absorptionImage);
		}
	}

}

void ImagePreprocessor::boundBelow(std::vector <double> & v, double bound)
{
	std::vector<double>::iterator it;

	for(it = v.begin(); it != v.end(); it++)
	{
		if (*it < bound)
		{
			*it = bound;
		}
	}

}

void ImagePreprocessor::boundAbove(std::vector <double> & v, double bound)
{
	std::vector<double>::iterator it;

	for(it = v.begin(); it != v.end(); it++)
	{
		if (*it > bound)
		{
			*it = bound;
		}
	}

}