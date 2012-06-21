#ifndef BLOCKINGQUEUE_H
#define	BLOCKINGQUEUE_H

#include <queue>
#include <cassert>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include "Semaphore.h"

template <typename T>
class BlockingQueue : boost::noncopyable
{
protected:
  std::queue<T> _queue;
  Semaphore _semaphore;
  std::size_t _maxQueueSize;
  typedef boost::recursive_mutex mutex;
  typedef boost::lock_guard<mutex> mutex_lock;
  mutex _mutex;
  bool _terminating;
public:
  BlockingQueue(std::size_t maxQueueSize) :
    _maxQueueSize(maxQueueSize),
    _terminating(false)
  {
  }

  ~BlockingQueue()
  {
    terminate();
  }

  void terminate()
  {
    //
    // Unblock dequeue
    //
    mutex_lock lock(_mutex);
    _terminating = true;
    _semaphore.signal();
  }

  bool dequeue(T& data)
  {
    _semaphore.wait();
    mutex_lock lock(_mutex);
    if (_queue.empty() || _terminating)
      return false;
    data = _queue.front();
    _queue.pop();
    return true;
  }
  
  void enqueue(const T& data)
  {
    _mutex.lock();
    if (_maxQueueSize && _queue.size() > _maxQueueSize)
      _queue.pop();
    _queue.push(data);
    _mutex.unlock();
    _semaphore.signal();
  }


};

#endif	/* BLOCKINGQUEUE_H */

