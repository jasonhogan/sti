
#include "QueuedEventHandler.h"

#include <vector>
#include <deque>
#include <iostream>

using namespace STI::Utils;


QueuedEventHandler::QueuedEventHandler(unsigned threadPoolSize, unsigned eventQueuePerThreadLimit) :
eventQueuePerThreadLimit_l(eventQueuePerThreadLimit), threadPoolSize_l(threadPoolSize)
{
	boost::shared_ptr<boost::thread> loopThread;
	
	eventLoopThreadCount = 0;
	running = true;

//	boost::thread thr1( &QueuedEventHandler::eventLoop, this );
//	boost::thread thr1(boost::bind( &QueuedEventHandler::eventLoop, this) );
//	thr1.join();

	while(loopThreads.size() < threadPoolSize)
	{
		loopThread = boost::shared_ptr<boost::thread>(
			new boost::thread(&QueuedEventHandler::eventLoop, this));

		loopThreads.push_back(loopThread);
	}
//	loopThreads.at(0)->join();
}

QueuedEventHandler::~QueuedEventHandler()
{
	{
		boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);
		running = false;
	}

	queueCondition.notify_all();

	bool waiting = true;
	while(waiting)
	{
		{
			boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);
			waiting = (eventLoopThreadCount > 0);
		}
		//for(unsigned i = 0; i < loopThreads.size(); i++) {
		//	waiting |= (loopThreads.at(i) != 0 && loopThreads.at(i)->joinable());
		//}
	}


	//for(unsigned i = 0; i < loopThreads.size(); i++) {
	//	if(loopThreads.at(i) != 0 && loopThreads.at(i)->joinable()) {
	//		loopThreads.at(i)->join();
	//	}
	//}

	//int x;
	//std::cin >> x;
}


void QueuedEventHandler::addEventHighPriority(QueuedEvent_ptr& evt) 
{
	//lock with more aggressive timeout
	{
		boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);
		events.push_front(evt);
	}
	queueCondition.notify_one();
}

void QueuedEventHandler::addEvent(QueuedEvent_ptr& evt) 
{
	//lock with timeout
	{
		boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);

		unsigned addEventSizeLimit = eventQueuePerThreadLimit_l * threadPoolSize_l;
		double emptyFraction = 1;	//Just to check if limit has been exceeded.
		
		while(events.size() > addEventSizeLimit * emptyFraction)
		{
			//Too many event have been added. Wait for some to be handled.
			emptyFraction = 0.5;	//wait until half have been handled.
			
			//Sleep for 500 ms
			boost::system_time wakeTime = boost::get_system_time() + boost::posix_time::milliseconds( static_cast<long>(500) );
			queueCondition.timed_wait(writeLock, wakeTime);
		}
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
	{
		boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);
		eventLoopThreadCount++;
	}

	QueuedEvent_ptr nextEvt;

	while(running)
	{
		boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);

		while( events.empty() && running)
		{
			queueCondition.wait(writeLock);
		}
		if(running) {
			nextEvt = events.front();	//QueuedEvent reference count = 2
			events.pop_front();			//QueuedEvent reference count = 1

			writeLock.unlock();	//Important so other events can be added while run() is running

			nextEvt->run();		
			nextEvt.reset();	//QueuedEvent reference count = 0 and memory is freed
		}
	}

	{
		boost::unique_lock< boost::shared_mutex > writeLock(queueMutex);
		eventLoopThreadCount--;
	}
}

