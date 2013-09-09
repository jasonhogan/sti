

#ifndef STI_SCOPE_COLLECTIONMODE_H
#define STI_SCOPE_COLLECTIONMODE_H

#include <string>
#include "MixedData.h"

namespace STI
{
namespace Scope
{

template<typename T, typename Y, typename Y_out=Y>
class CollectionMode
{
public:
	std::string mode;
	double modeParameter;

	virtual void processData(MixedData& dataOut, Y* dataIn, unsigned lengthDataIn, T timeInterval, unsigned downsample) = 0;
	
	void processData(MixedData& dataOut, std::vector<Y>& dataIn, T timeInterval, unsigned downsample);

	static Y averagePoints(Y* dataIn, unsigned begin, unsigned end);
};


template<typename T, typename Y, typename Y_out=Y>
class ThresholdMode : public CollectionMode<T, Y, Y_out>
{
public:

	void processData(MixedData& dataOut, Y* dataIn, unsigned lengthDataIn, T timeInterval, unsigned downsample);

private:

	virtual bool keepDataPoint(const Y& value) const = 0;
};

template<typename T, typename Y, typename Y_out=Y>
class NormalMode : public ThresholdMode<T, Y, Y_out>
{
	bool keepDataPoint(const Y& value) const { return true; }
};

template<typename T, typename Y, typename Y_out=Y>
class ThresholdModeUpper : public ThresholdMode<T, Y, Y_out>
{
	bool keepDataPoint(const Y& value) const { return (value > modeParameter); }
};

template<typename T, typename Y, typename Y_out=Y>
class ThresholdModeLower : public ThresholdMode<T, Y, Y_out>
{
	bool keepDataPoint(const Y& value) const { return (value < modeParameter); }
};

}
}

//Compatibility with vector. Forwards to the array version of processData.
template<typename T, typename Y, typename Y_out>
void STI::Scope::CollectionMode<T,Y,Y_out>::processData(MixedData& dataOut, std::vector<Y>& dataIn, T timeInterval, unsigned downsample)
{
	if(dataIn.size() > 0) {
		//Get the underlying array from the vector.
		processData(dataOut, &dataIn[0], dataIn.size(), timeInterval, downsample);
	}
}


//Average the values of dataIn in the index range [begin, end), including begin and excluding end.
template<typename T, typename Y, typename Y_out>
Y STI::Scope::CollectionMode<T,Y,Y_out>::averagePoints(Y* dataIn, unsigned begin, unsigned end)
{
	if(begin >= end) {
		//Perverse case
		end = begin + 1;
	}

	Y total = 0;

	for(unsigned i = begin; i < end; i++) {
		total += dataIn[i];
	}

	return total / (end - begin);
}

template<typename T, typename Y, typename Y_out>
void STI::Scope::ThresholdMode<T,Y,Y_out>::processData(MixedData& dataOut, Y* dataIn, unsigned lengthDataIn, T timeInterval, unsigned downsample)
{
	//Sanitize downsample parameter
	if(downsample > lengthDataIn) {
		//For rare, perverse situations...
		downsample = lengthDataIn;
	}

	Y average;
	unsigned sectionIndex = 0;
	bool firstPoint = true;
	
	dataOut.clear();

	//Downsample and threshold, dividing data into sections of continuous data tagged by
	//the initial time of the section.
	for(unsigned i = 0; i < lengthDataIn; i += downsample) {
		
		average = averagePoints(dataIn, i, i + downsample);		//downsample the data

		if (keepDataPoint(average)) {
			if (firstPoint) {
				//Each section is a of vector length 2:  {initial time, {values} } 
				if(dataOut.getVector().size() > 0) {	//skip the increment for the first section
					sectionIndex++;
				}
				dataOut.addValue(MixedData());									//add new section
				dataOut.getValueAt(sectionIndex).addValue(static_cast<Y_out>(i * timeInterval));	//Start time of section
				dataOut.getValueAt(sectionIndex).addValue(MixedData());			//vector of values in section
				
				firstPoint = false;
			}
			
			//Append to the current vector of values
			dataOut.getValueAt(sectionIndex).getValueAt(1).addValue(static_cast<Y_out>(average));
		}
		else {
			firstPoint = true;		//Next time threshold criteria is met a new section will be made.
		}
	}
}


//template<typename T, typename Y>
//void STI::Scope::NormalMode<T,Y>::processData(MixedData& dataOut, std::vector<Y>& dataIn, T timeInterval, unsigned downsample)
//{
//	dataOut.clear();
//
//	MixedData dataOutElement;
//
//	cout << "Normal Mode: processing data" << endl;
//	dataOutElement.addValue(0.0);
//	dataOutElement.addValue(dataIn);
//
//	dataOut.addValue(dataOutElement);
//
//}

//
//template<typename T, typename Y>
//void STI::Scope::ThresholdModeLower<T,Y>::processData(MixedData& dataOut, std::vector <Y>& dataIn, T timeInterval)
//{
//	dataOut.clear();
//
//	bool savedPreviousPoint = false;
//	std::vector<Y> tempDataVector;
//	MixedData dataOutElement;
//	for (unsigned int i = 0; i < dataIn.size(); i++)
//	{
//		if (dataIn.at(i) < modeParameter)
//		{
//			tempDataVector.push_back(dataIn.at(i));
//			
//			if (!savedPreviousPoint)
//			{
//				dataOutElement.addValue(i*timeInterval);
//				savedPreviousPoint = true;
//			}
//
//		}
//		else
//		{
//			if (savedPreviousPoint)
//			{
//				dataOutElement.addValue(tempDataVector);
//				tempDataVector.clear();
//
//				dataOut.addValue(dataOutElement);
//				dataOutElement.clear();
//
//				savedPreviousPoint = false;
//			}
//		}
//	}
//
//}
//


#endif
