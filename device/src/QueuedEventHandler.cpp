
#include "QueuedEventHandler.h"

#include <vector>
#include <deque>

using namespace STI::Utils;


QueuedEventHandler::QueuedEventHandler(unsigned threadPoolSize)
{
	boost::shared_ptr<boost::thread> loopThread;

	running = true;

	while(loopThreads.size() < threadPoolSize)
	{
		loopThread = boost::shared_ptr<boost::thread>(
			new boost::thread(&QueuedEventHandler::eventLoop, this));

		loopThreads.push_back(loopThread);
	}
}

QueuedEventHandler::~QueuedEventHandler()
{
	{
		boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);
		running = false;
	}

	queueCondition.notify_all();

	for(unsigned i = 0; i < loopThreads.size(); i++) {
		loopThreads.at(i)->join();
	}
}


void QueuedEventHandler::addEventHighPriority(boost::shared_ptr<QueuedEvent>& evt) 
{
	//lock with more aggressive timeout
	{
		boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);
		events.push_front(evt);
	}
	queueCondition.notify_one();
}

void QueuedEventHandler::addEvent(boost::shared_ptr<QueuedEvent>& evt) 
{
	//lock with timeout
	{
		boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);
		events.push_back(evt);
	}

	queueCondition.notify_one();
}

void QueuedEventHandler::cancelAllEvents()
{
	boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);
	events.clear();
}

void QueuedEventHandler::eventLoop()
{
	boost::shared_ptr<QueuedEvent> nextEvt;

	while(running)
	{
		boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);

		while( events.empty() )
		{
			queueCondition.wait(writeLock);
		}
			
		nextEvt = events.front();	//QueuedEvent reference count = 2
		events.pop_front();			//QueuedEvent reference count = 1

		writeLock.unlock();	//Important so other events can be added while run() is running

		nextEvt->run();		
		nextEvt.reset();	//QueuedEvent reference count = 0 and memory is freed
	}
}

