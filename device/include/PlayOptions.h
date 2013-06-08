#ifndef STI_TIMINGENGINE_PLAYOPTIONS_H
#define STI_TIMINGENGINE_PLAYOPTIONS_H

namespace STI
{
namespace TimingEngine
{


class PlayOptions
{
public:
	PlayOptions(double StartTime, double EndTime, short Repeats)
		: startTime(StartTime), endTime(EndTime), repeats(Repeats) {}
	
	double startTime;
	double endTime; 
	short repeats;
};

}
}

#endif

