/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.comm.io;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.Condition;
/**
 *
 * @author Jason Hogan
 * 10/28/2010
 */
public abstract class QueuedEventHandler<QueuedEvent> implements Runnable {

    private LinkedBlockingQueue<QueuedEvent> eventQueue = new LinkedBlockingQueue<QueuedEvent>();
    private ReentrantLock eventReLock = new ReentrantLock();
    private Condition eventCondition = eventReLock.newCondition();

    private boolean running = true;

    public abstract void handleEvent(QueuedEvent event);

    public void putEvent(QueuedEvent event) {
        // Makes sure messages are printing in the order they are received.
        eventReLock.lock();
        try {
            eventQueue.put(event);
            eventCondition.signalAll();
        } catch (InterruptedException ex) {
        } finally {
            eventReLock.unlock();
        }
    }

    public void run() {
        while (running) {
            //if queue is empty, sleep until needed
            eventReLock.lock();
            try {
                if (eventQueue.size() == 0) {
                    eventCondition.await();
                }
            } catch (InterruptedException ex) {
            } finally {
                eventReLock.unlock();
            }

            //send events in queue
            while (eventQueue.size() > 0) {
                try {
                    handleEvent(eventQueue.take());
                } catch (InterruptedException ex) {
                }
            }
        }

    }
}
