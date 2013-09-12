#ifndef STI_UTILS_QUEUEDEVENTHANDLER
#define STI_UTILS_QUEUEDEVENTHANDLER

//#include <boost/thread/locks.hpp>
//#include <boost/thread/shared_mutex.hpp>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <vector>
#include <deque>

namespace STI
{
namespace Utils
{

class QueuedEvent;
typedef boost::shared_ptr<boost::thread> ThreadPtr;
typedef boost::shared_ptr<QueuedEvent> QueuedEvent_ptr;
class QueuedEventHandler;
typedef boost::shared_ptr<QueuedEventHandler> QueuedEventHandler_ptr;

class QueuedEventHandler
{
public:

	QueuedEventHandler(unsigned threadPoolSize, unsigned eventQueuePerThreadLimit=1000);
	~QueuedEventHandler();

	void addEventHighPriority(QueuedEvent_ptr& evt);
	void addEvent(QueuedEvent_ptr& evt);
	void cancelAllEvents();

private:

	void eventLoop();

	bool running;
	int eventLoopThreadCount;

	unsigned eventQueuePerThreadLimit_l;	//Max number of events in the Queue before addEvent will be delayed.
	unsigned threadPoolSize_l;

	std::vector<ThreadPtr> loopThreads;

	std::deque<QueuedEvent_ptr> events;
	
	mutable boost::shared_mutex queueMutex;
	boost::condition_variable_any queueCondition;

};

//All events in the queue decend from this class and implement run()
class QueuedEvent
{
public:
	virtual ~QueuedEvent() {}

	virtual void run() = 0;
};

}
}

#endif

