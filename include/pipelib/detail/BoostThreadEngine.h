/*
 * BoostThreadEngine.h
 *
 *
 *  Created on: Sep 6, 2013
 *      Author: Martin Uhrin
 */

#ifndef BOOST_THREAD_ENGINE_DETAIL_H
#define BOOST_THREAD_ENGINE_DETAIL_H

// INCLUDES /////////////////////////////////////////////

#include "pipelib/Barrier.h"
#include "pipelib/event/EventSupport.h"
#include "pipelib/event/PipeEngineEvents.h"
#include "pipelib/event/PipeEngineListener.h"

namespace pipelib {

template< typename Pipe, typename Shared, typename Global>
  BoostThreadEngine< Pipe, Shared, Global>::BoostThreadEngine(
      const size_t numThreads) :
      myNumThreads(numThreads), myRoot(this), myMaxReleases(
          DEFAULT_MAX_RELEASES)
  {
    init();
  }

template< typename Pipe, typename Shared, typename Global>
  BoostThreadEngine< Pipe, Shared, Global>::BoostThreadEngine(
      const size_t numThreads, BlockHandleType & startBlock) :
      myNumThreads(numThreads), myRoot(this), myMaxReleases(
          DEFAULT_MAX_RELEASES)
  {
    init();
    attach(startBlock);
  }

template< typename Pipe, typename Shared, typename Global>
  BoostThreadEngine< Pipe, Shared, Global>::~BoostThreadEngine()
  {
    myEventSupport.notify(event::makeDestroyedEvent(*this));
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::attach(BlockHandleType & startBlock)
  {
    PIPELIB_ASSERT(startBlock->asStartBlock());

    if(isAttached())
      detach();

    myStartBlock = startBlock;
    ::std::for_each(myStartBlock->beginPreorder(), myStartBlock->endPreorder(),
        ::boost::bind(&Self::notifyAttached, this, _1, this));
  }

template< typename Pipe, typename Shared, typename Global>
  bool
  BoostThreadEngine< Pipe, Shared, Global>::detach()
  {
    if(!isAttached())
      return false;

    ::std::for_each(myStartBlock->beginPreorder(), myStartBlock->endPreorder(),
        ::boost::bind(&Self::notifyDetached, this, _1));
    clear();
    return true;
  }

template< typename Pipe, typename Shared, typename Global>
  bool
  BoostThreadEngine< Pipe, Shared, Global>::isAttached() const
  {
    return myStartBlock.get() != NULL;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::run()
  {
    PIPELIB_ASSERT(isAttached());

    changeState(PipelineState::INITIALISED);
    changeState(PipelineState::RUNNING);
    changeState(PipelineState::FINISHED);
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::run(BlockHandleType & startBlock)
  {
    PIPELIB_ASSERT(startBlock->asStartBlock());

    attach(startBlock);
    run();
    detach();
  }

template< typename Pipe, typename Shared, typename Global>
  PipelineState::Value
  BoostThreadEngine< Pipe, Shared, Global>::getState() const
  {
    return myState;
  }

template< typename Pipe, typename Shared, typename Global>
  const typename BoostThreadEngine< Pipe, Shared, Global>::BlockHandleType &
  BoostThreadEngine< Pipe, Shared, Global>::getStartBlock() const
  {
    return myStartBlock;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::setFinishedDataSink(
      FinishedSinkType * sink)
  {
    myFinishedSink = sink;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::setDroppedDataSink(
      DroppedSinkType * sink)
  {
    myDroppedSink = sink;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::addListener(ListenerType & listener)
  {
    myEventSupport.insert(listener);
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::removeListener(
      ListenerType & listener)
  {
    myEventSupport.remove(listener);
  }

template< typename Pipe, typename Shared, typename Global>
  Shared &
  BoostThreadEngine< Pipe, Shared, Global>::sharedData()
  {
    return *myShared;
  }

template< typename Pipe, typename Shared, typename Global>
  const Shared &
  BoostThreadEngine< Pipe, Shared, Global>::sharedData() const
  {
    return *myShared;
  }

template< typename Pipe, typename Shared, typename Global>
  Global &
  BoostThreadEngine< Pipe, Shared, Global>::globalData()
  {
    return *myRoot->myGlobal;
  }

template< typename Pipe, typename Shared, typename Global>
  const Global &
  BoostThreadEngine< Pipe, Shared, Global>::globalData() const
  {
    return *myGlobal;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::out(Pipe * data,
      const BlockType & outBlock, const Channel channel)
  {
    const BlockHandleType & inBlock = outBlock.getOutput(channel);
    if(inBlock.get())
      postTask(::boost::bind(&Self::outTask, this, data, inBlock->asPipeBlock()));
    else
    {
      // So this data is finished, check if we have a sink, otherwise delete
      typename DataStore::iterator it = findData(data);

      PIPELIB_ASSERT_MSG(it != myDataStore.end(),
          "Couldn't find data in data store");

      if(myFinishedSink)
        myFinishedSink->finished(
            makeUniquePtr(myDataStore.release(it).release()));
      else
        myDataStore.erase(it);
    }
  }

template< typename Pipe, typename Shared, typename Global>
  Pipe *
  BoostThreadEngine< Pipe, Shared, Global>::createData()
  {
    myDataStore.push_back(new Pipe());
    return &myDataStore.back();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::dropData(Pipe * data)
  {
    // So this data is finished, check if we have a sink, otherwise delete
    typename DataStore::iterator it = findData(data);

    PIPELIB_ASSERT_MSG(it != myDataStore.end(),
        "Couldn't find data in data store");

    if(myDroppedSink)
      myDroppedSink->dropped(makeUniquePtr(myDataStore.release(it).release()));
    else
      myDataStore.erase(it);
  }

template< typename Pipe, typename Shared, typename Global>
  Pipe *
  BoostThreadEngine< Pipe, Shared, Global>::registerData(PipePtr data)
  {
    myDataStore.push_back(data.release());
    return &myDataStore.back();
  }

template< typename Pipe, typename Shared, typename Global>
  typename BoostThreadEngine< Pipe, Shared, Global>::Base *
  BoostThreadEngine< Pipe, Shared, Global>::createEngine()
  {
    myChildren.push_back(new BoostThreadEngine(this));
    return &myChildren.back();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::releaseEngine(Base * engine)
  {
    bool found = false;
    for(typename ChildEngines::iterator it = myChildren.begin(), end =
        myChildren.end(); it != end; ++it)
    {
      if(&(*it) == engine)
      {
        myChildren.erase(it);
        found = true;
        break;
      }
    }
    PIPELIB_ASSERT_MSG(found, "Engine not found");
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::registerBarrier(
      BarrierType * const barrier)
  {
    myBarriers.push_back(barrier);
  }

template< typename Pipe, typename Shared, typename Global>
  BoostThreadEngine< Pipe, Shared, Global>::BoostThreadEngine(Self * root) :
      myRoot(root), myMaxReleases(DEFAULT_MAX_RELEASES),
      myNumThreads(0) // Only root has threads
  {
    init();
  }

template< typename Pipe, typename Shared, typename Global>
  BoostThreadEngine< Pipe, Shared, Global>::BoostThreadEngine(Self * root,
      BlockHandleType & pipe) :
      myRoot(root), myMaxReleases(DEFAULT_MAX_RELEASES),
      myNumThreads(0) // Only root has threads
  {
    init();
    attach(pipe);
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::init()
  {
    myFinishedSink = NULL;
    myDroppedSink = NULL;
    clear();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::doRun()
  {
    namespace asio = ::boost::asio;

    if(isRoot())
    {
      myThreading.reset(new Threading());

      for(size_t i = 1; i < myNumThreads; ++i)
        myThreading->threads.create_thread(
            ::boost::bind(&asio::io_service::run, &myThreading->threadService));
    }

    postTask(::boost::bind(&Self::startTask, this, myStartBlock->asStartBlock()));

    runTillFinished();

    // Release any barriers that are waiting
    unsigned int numReleases = 0;
    while(releaseNextBarrier())
    {
      runTillFinished();

      ++numReleases;
      if(numReleases >= myMaxReleases)
        break;
    }

    if(isRoot())
    {
      myThreading->threads.join_all();
      myThreading.reset();
    }
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::changeState(
      const PipelineState::Value newState)
  {
    const PipelineState::Value oldState = myState;
    switch (newState)
    {
    case PipelineState::INITIALISED:

      // Tell the blocks
      ::std::for_each(myStartBlock->beginPreorder(),
          myStartBlock->endPreorder(),
          ::boost::bind(&Self::notifyInitialising, this, _1, this));

      // Sort the barriers so when we release at the end they're in the correct order
      this->sortBarriers(&myBarriers);

      myState = PipelineState::INITIALISED;
      ::std::for_each(myStartBlock->beginPreorder(),
          myStartBlock->endPreorder(),
          ::boost::bind(&Self::notifyInitialised, this, _1));

      // Tell any listeners
      myEventSupport.notify(
          event::makeStateChangedEvent(*this, oldState, myState));
      break;

    case PipelineState::RUNNING:

      ::std::for_each(myStartBlock->beginPreorder(),
          myStartBlock->endPreorder(),
          ::boost::bind(&Self::notifyStarting, this, _1));
      myState = PipelineState::RUNNING;
      myEventSupport.notify(
          event::makeStateChangedEvent(*this, oldState, myState));
      doRun();

      break;
    case PipelineState::STOPPED:
      myState = PipelineState::STOPPED;
      // Tell any listeners
      myEventSupport.notify(
          event::makeStateChangedEvent(*this, oldState, myState));
      break;
    case PipelineState::FINISHED:

      ::std::for_each(myStartBlock->beginPreorder(),
          myStartBlock->endPreorder(),
          ::boost::bind(&Self::notifyFinishing, this, _1));
      myState = PipelineState::FINISHED;
      myShared.reset(new Shared());
      ::std::for_each(myStartBlock->beginPreorder(),
          myStartBlock->endPreorder(),
          ::boost::bind(&Self::notifyFinished, this, _1, this));

      // Tell any listeners
      myEventSupport.notify(
          event::makeStateChangedEvent(*this, oldState, myState));
      break;
    }
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::clear()
  {
    myStartBlock.reset();
    if(isRoot())
      myGlobal.reset(new Global());
    myShared.reset(new Shared());
    myState = PipelineState::UNINITIALISED;
    myChildren.clear();
    myDataStore.clear();
    myBarriers.clear();
    myMaxReleases = DEFAULT_MAX_RELEASES;
    myNumRunning = 0;
  }

template< typename Pipe, typename Shared, typename Global>
  bool
  BoostThreadEngine< Pipe, Shared, Global>::releaseNextBarrier()
  {
    BOOST_FOREACH(BarrierType * const barrier, myBarriers)
    {
      if(barrier->hasData())
      {
        postTask(::boost::bind(&Self::releaseBarrierTask, this, barrier));
        return true;
      }
    }
    return false;
  }

template< typename Pipe, typename Shared, typename Global>
  typename BoostThreadEngine< Pipe, Shared, Global>::DataStore::iterator
  BoostThreadEngine< Pipe, Shared, Global>::findData(const Pipe * const data)
  {
    typename DataStore::iterator it, end = myDataStore.end();
    for(it = myDataStore.begin(); it != end; ++it)
    {
      if(&(*it) == data)
        break;
    }
    return it;
  }

template< typename Pipe, typename Shared, typename Global>
  typename BoostThreadEngine< Pipe, Shared, Global>::DataStore::const_iterator
  BoostThreadEngine< Pipe, Shared, Global>::findData(
      const Pipe * const data) const
  {
    typename DataStore::const_iterator it, end = myDataStore.end();
    for(it = myDataStore.begin(); it != end; ++it)
    {
      if(&(*it) == data)
        break;
    }
    return it;
  }

template< typename Pipe, typename Shared, typename Global>
  bool
  BoostThreadEngine< Pipe, Shared, Global>::isRoot() const
  {
    return myRoot == this;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::runTillFinished() const
  {
    while(myNumRunning > 0)
    {
      if(myRoot->myThreading->threadService.stopped())
        myRoot->myThreading->threadService.reset();

      const size_t numRan = myRoot->myThreading->threadService.run_one();
      if(numRan != 0)
        ::boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
  }

template< typename Pipe, typename Shared, typename Global>
void
BoostThreadEngine < Pipe, Shared, Global>::incrementNumRunning()
{
  ::boost::lock_guard< ::boost::mutex> guard(myNumRunningMutex);
  ++myNumRunning;
}

template< typename Pipe, typename Shared, typename Global>
void
BoostThreadEngine < Pipe, Shared, Global>::decrementNumRunning()
{
  ::boost::lock_guard< ::boost::mutex> guard(myNumRunningMutex);
  --myNumRunning;
}

template< typename Pipe, typename Shared, typename Global>
template<typename Task>
void
BoostThreadEngine < Pipe, Shared, Global>::postTask(Task task)
{
  incrementNumRunning();
  myRoot->myThreading->threadService.post(task);
}

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::startTask(
      StartBlockType * const startBlock)
  {
    startBlock->start();
    decrementNumRunning();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::outTask(Pipe * const data,
      PipeBlockType * const inBlock)
  {
    inBlock->in(data);
    decrementNumRunning();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  BoostThreadEngine< Pipe, Shared, Global>::releaseBarrierTask(
      BarrierType * const barrier)
  {
    barrier->release();
    decrementNumRunning();
  }

}

#endif /* BOOST_THREAD_ENGINE_DETAIL_H */

