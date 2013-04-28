#ifndef STI_TIMINGENGINE_MASTERTRIGGER_H
#define STI_TIMINGENGINE_MASTERTRIGGER_H


namespace STI
{
namespace TimingEngine
{

class MasterTrigger
{
public:

	virtual void triggerAll() = 0;

//	sequence<TTrigger> triggers;
//	TRepeatMode mode; // {Single, Continuous}

};

}
}

#endif
