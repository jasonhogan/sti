
#include "QueuedEventHandler.h"

#include <vector>
#include <deque>

using namespace STI::Utils;


QueuedEventHandler::QueuedEventHandler(unsigned threadPoolSize)
{
	boost::shared_ptr<boost::thread> loopThread;
	
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

	for(unsigned i = 0; i < loopThreads.size(); i++) {
		if(loopThreads.at(i) != 0 && loopThreads.at(i)->joinable()) {
			loopThreads.at(i)->join();
		}
	}
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
}

