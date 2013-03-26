#ifndef STI_UTILS_QUEUEDEVENTHANDLER
#define STI_UTILS_QUEUEDEVENTHANDLER

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <vector>
#include <deque>

namespace STI
{
namespace Utils
{

class QueuedEventHandler
{
public:

	class QueuedEvent;
	typedef boost::shared_ptr<boost::thread> ThreadPtr;

	QueuedEventHandler(unsigned threadPoolSize);
	~QueuedEventHandler();

	void addEventHighPriority(boost::shared_ptr<QueuedEvent>& evt);
	void addEvent(boost::shared_ptr<QueuedEvent>& evt);
	void cancelAllEvents();

	//All events in the queue decend from this class and implement run()
	class QueuedEvent
	{
	public:
		virtual void run() = 0;
	};

private:

	void eventLoop();

	bool running;

	std::vector<ThreadPtr> loopThreads;

	std::deque< boost::shared_ptr<QueuedEvent> > events;
	
	mutable boost::shared_mutex queueMutex;
	boost::condition_variable_any queueCondition;

};

}
}

#endif

