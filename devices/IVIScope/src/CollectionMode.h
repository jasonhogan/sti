

#ifndef STI_SCOPE_COLLECTIONMODE_H
#define STI_SCOPE_COLLECTIONMODE_H

#include <string>
#include "MixedData.h"

namespace STI
{
namespace Scope
{

template<typename T, typename Y>
class CollectionMode
{
public:
	std::string mode;
	double modeParameter;

	virtual void processData(MixedData& dataOut, std::vector<Y>& dataIn, T timeInterval) = 0;
};

template<typename T, typename Y>
class NormalMode : public CollectionMode<T, Y>
{
	void processData(MixedData& dataOut, std::vector<Y>& dataIn, T timeInterval);
};

template<typename T, typename Y>
class ThresholdModeUpper : public CollectionMode<T, Y>
{
	void processData(MixedData& dataOut, std::vector<Y>& dataIn, T timeInterval);
};

template<typename T, typename Y>
class ThresholdModeLower : public CollectionMode<T, Y>
{
	void processData(MixedData& dataOut, std::vector<Y>& dataIn, T timeInterval);
};

}
}

template<typename T, typename Y>
void STI::Scope::NormalMode<T,Y>::processData(MixedData& dataOut, std::vector<Y>& dataIn, T timeInterval)
{
	dataOut.clear();

	MixedData dataOutElement;

	cout << "Normal Mode: processing data" << endl;
	dataOutElement.addValue(0.0);
	dataOutElement.addValue(dataIn);

	dataOut.addValue(dataOutElement);

}

template<typename T, typename Y>
void STI::Scope::ThresholdModeUpper<T,Y>::processData(MixedData& dataOut, std::vector <Y>& dataIn, T timeInterval)
{
	dataOut.clear();

	bool savedPreviousPoint = false;
	std::vector<Y> tempDataVector;
	MixedData dataOutElement;
	for (unsigned int i = 0; i < dataIn.size(); i++)
	{
		if (dataIn.at(i) > modeParameter)
		{
			tempDataVector.push_back(dataIn.at(i));
			
			if (!savedPreviousPoint)
			{
				dataOutElement.addValue(i*timeInterval);
				savedPreviousPoint = true;
			}

		}
		else
		{
			if (savedPreviousPoint)
			{
				dataOutElement.addValue(tempDataVector);
				tempDataVector.clear();

				dataOut.addValue(dataOutElement);
				dataOutElement.clear();

				savedPreviousPoint = false;
			}
		}
	}

}

template<typename T, typename Y>
void STI::Scope::ThresholdModeLower<T,Y>::processData(MixedData& dataOut, std::vector <Y>& dataIn, T timeInterval)
{
	dataOut.clear();

	bool savedPreviousPoint = false;
	std::vector<Y> tempDataVector;
	MixedData dataOutElement;
	for (unsigned int i = 0; i < dataIn.size(); i++)
	{
		if (dataIn.at(i) < modeParameter)
		{
			tempDataVector.push_back(dataIn.at(i));
			
			if (!savedPreviousPoint)
			{
				dataOutElement.addValue(i*timeInterval);
				savedPreviousPoint = true;
			}

		}
		else
		{
			if (savedPreviousPoint)
			{
				dataOutElement.addValue(tempDataVector);
				tempDataVector.clear();

				dataOut.addValue(dataOutElement);
				dataOutElement.clear();

				savedPreviousPoint = false;
			}
		}
	}

}



#endif
