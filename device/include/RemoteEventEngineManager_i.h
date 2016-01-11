

namespace STI
{
namespace TimingEngine
{


//Device-side; just forwards network calls to the local implementation of EventEngineManager
class RemoteEventEngineManager_i : public POA_STI::Server_Device::RemoteEventEngineManager
{
	RemoteEventEngineManager_i(EventEngineManager* eventEngineManager);
	~RemoteEventEngineManager_i();

	//void load(STI::Types::TEngineID engineID)
	//{
	//	unique_ptr<EngineID> id;
	//	if( STI::Utils::convert(engineID, id) )
	//		engineManager->load(id);
	//}

	::CORBA::Boolean lock(const Types::TEngineID& engineID);
	void unlock(const Types::TEngineID& engineID);
	void unlockAll();
	void delegateTrigger(const Types::TEngineID& engineID, RemoteTrigger_ptr systemTrigger);
	void removeTrigger(const Types::TEngineID& engineID);
	::CORBA::Boolean addEventEngine(const Types::TEngineID& engineID);
	::CORBA::Boolean hasEngine(const Types::TEngineID& engineID);
	void removeAllEngines();
	Types::TEngineIDSeq* getEngineIDs();
	Types::TEventEngineStatus getStatus(const Types::TEngineID& engineID);
	Types::TEventEngineState getState(const Types::TEngineID& engineID);
	void clear(const Types::TEngineID& engineID);
	void parse(const Types::TEngineID& engineID, const Types::TEventSeq& eventsIn, ParsingResultsHandler_ptr results);
	void load(const Types::TEngineID& engineID);
	void play(const Types::TEngineInstance& engineInstance, ::CORBA::Double startTime, ::CORBA::Double endTime, ::CORBA::Short repeats, ::CORBA::Double repeatTime, const Types::TDocumentationOptions& docOptions);
	void playAll(const Types::TEngineInstance& engineInstance, const Types::TDocumentationOptions& docOptions);
	void trigger();
	void pause(const Types::TEngineID& engineID);
	void pauseAt(const Types::TEngineID& engineID, ::CORBA::Double time);
	void resume(const Types::TEngineID& engineID);
	void resumeAt(const Types::TEngineID& engineID, ::CORBA::Double newTime);
	void stop(const Types::TEngineID& engineID);
	::CORBA::Boolean collectData(const Types::TEngineInstance& engineInstance, Types::TMeasurementSeq_out data);

};

}
}
