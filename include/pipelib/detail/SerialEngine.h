/*
 * SerialEngine.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SERIAL_ENGINE_DETAIL_H
#define SERIAL_ENGINE_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <algorithm>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "pipelib/Barrier.h"
#include "pipelib/event/EventSupport.h"
#include "pipelib/event/PipeEngineEvents.h"
#include "pipelib/event/PipeEngineListener.h"

#ifdef _MSC_VER
// Disable warning about passing this pointer in initialisation list
#  pragma warning( push )
#  pragma warning( disable : 4355 )
#endif

namespace pipelib {

template< typename Pipe, typename Shared, typename Global>
  SerialEngine< Pipe, Shared, Global>::SerialEngine() :
      myGlobal(new Global()), myIsRoot(true), myMaxReleases(DEFAULT_MAX_RELEASES)
  {
    init();
  }

template< typename Pipe, typename Shared, typename Global>
  SerialEngine< Pipe, Shared, Global>::SerialEngine(BlockHandleType & startBlock) :
      myGlobal(new Global()), myIsRoot(true), myMaxReleases(DEFAULT_MAX_RELEASES)
  {
    init();
    attach(startBlock);
  }

template< typename Pipe, typename Shared, typename Global>
  SerialEngine< Pipe, Shared, Global>::~SerialEngine()
  {
    myEventSupport.notify(event::makeDestroyedEvent(*this));
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::attach(BlockHandleType & startBlock)
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
  SerialEngine< Pipe, Shared, Global>::detach()
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
  SerialEngine< Pipe, Shared, Global>::isAttached() const
  {
    return myStartBlock.get() != NULL;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::run()
  {
    PIPELIB_ASSERT(isAttached());

    changeState(PipelineState::INITIALISED);
    changeState(PipelineState::RUNNING);
    changeState(PipelineState::FINISHED);
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::run(BlockHandleType & startBlock)
  {
    PIPELIB_ASSERT(startBlock->asStartBlock());

    attach(startBlock);
    run();
    detach();
  }

template< typename Pipe, typename Shared, typename Global>
  PipelineState::Value
  SerialEngine< Pipe, Shared, Global>::getState() const
  {
    return myState;
  }

template< typename Pipe, typename Shared, typename Global>
const typename SerialEngine< Pipe, Shared, Global>::BlockHandleType &
  SerialEngine< Pipe, Shared, Global>::getStartBlock() const
  {
    return myStartBlock;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::setFinishedDataSink(FinishedSinkType * sink)
  {
    myFinishedSink = sink;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::setDroppedDataSink(DroppedSinkType * sink)
  {
    myDroppedSink = sink;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::addListener(ListenerType & listener)
  {
    myEventSupport.insert(listener);
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::removeListener(ListenerType & listener)
  {
    myEventSupport.remove(listener);
  }

template< typename Pipe, typename Shared, typename Global>
  Shared &
  SerialEngine< Pipe, Shared, Global>::sharedData()
  {
    return *myShared;
  }

template< typename Pipe, typename Shared, typename Global>
  const Shared &
  SerialEngine< Pipe, Shared, Global>::sharedData() const
  {
    return *myShared;
  }

template< typename Pipe, typename Shared, typename Global>
  Global &
  SerialEngine< Pipe, Shared, Global>::globalData()
  {
    return *myGlobal;
  }

template< typename Pipe, typename Shared, typename Global>
  const Global &
  SerialEngine< Pipe, Shared, Global>::globalData() const
  {
    return *myGlobal;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::out(Pipe * data, const BlockType & outBlock,
      const Channel channel)
  {
    const BlockHandleType & inBlock = outBlock.getOutput(channel);
    if(inBlock.get())
      inBlock->asPipeBlock()->in(data);
    else
    {
      // So this data is finished, check if we have a sink, otherwise delete
      typename DataStore::iterator it = findData(data);

      PIPELIB_ASSERT_MSG(it != myDataStore.end(), "Couldn't find data in data store");

      if(myFinishedSink)
        myFinishedSink->finished(makeUniquePtr(myDataStore.release(it).release()));
      else
        myDataStore.erase(it);
    }
  }

template< typename Pipe, typename Shared, typename Global>
  Pipe *
  SerialEngine< Pipe, Shared, Global>::createData()
  {
    myDataStore.push_back(new Pipe());
    return &myDataStore.back();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::dropData(Pipe * data)
  {
    // So this data is finished, check if we have a sink, otherwise delete
    typename DataStore::iterator it = findData(data);

    PIPELIB_ASSERT_MSG(it != myDataStore.end(), "Couldn't find data in data store");

    if(myDroppedSink)
      myDroppedSink->dropped(makeUniquePtr(myDataStore.release(it).release()));
    else
      myDataStore.erase(it);
  }

template< typename Pipe, typename Shared, typename Global>
  Pipe *
  SerialEngine< Pipe, Shared, Global>::registerData(PipePtr data)
  {
    myDataStore.push_back(data.release());
    return &myDataStore.back();
  }

template< typename Pipe, typename Shared, typename Global>
  typename SerialEngine< Pipe, Shared, Global>::Base *
  SerialEngine< Pipe, Shared, Global>::createEngine()
  {
    myChildren.push_back(new SerialEngine(myGlobal));
    return &myChildren.back();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::releaseEngine(Base * engine)
  {
    bool found = false;
    for(typename ChildEngines::iterator it = myChildren.begin(), end = myChildren.end(); it != end;
        ++it)
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
  SerialEngine< Pipe, Shared, Global>::registerBarrier(BarrierType * const barrier)
  {
    myBarriers.push_back(barrier);
  }

template< typename Pipe, typename Shared, typename Global>
  SerialEngine< Pipe, Shared, Global>::SerialEngine(::boost::shared_ptr<Global> & global) :
      myGlobal(global), myIsRoot(false), myMaxReleases(DEFAULT_MAX_RELEASES)
  {
    init();
  }

template< typename Pipe, typename Shared, typename Global>
  SerialEngine< Pipe, Shared, Global>::SerialEngine(::boost::shared_ptr<Global> & global,
      BlockHandleType & pipe) :
      myGlobal(global), myIsRoot(false), myMaxReleases(DEFAULT_MAX_RELEASES)
  {
    init();
    attach(pipe);
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::init()
  {
    myFinishedSink = NULL;
    myDroppedSink = NULL;
    clear();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::doRun()
  {
    myStartBlock->asStartBlock()->start();

    // Release any barriers that are waiting
    unsigned int numReleases = 0;
    while(releaseNextBarrier())
    {
      ++numReleases;
      if(numReleases >= myMaxReleases)
        break;
    }
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::changeState(const PipelineState::Value newState)
  {
    const PipelineState::Value oldState = myState;
    switch (newState)
    {
    case PipelineState::INITIALISED:

      // Tell the blocks
      ::std::for_each(myStartBlock->beginPreorder(), myStartBlock->endPreorder(),
          ::boost::bind(&Self::notifyInitialising, this, _1, this));

      // Sort the barriers so when we release at the end they're in the correct order
      this->sortBarriers(&myBarriers);

      myState = PipelineState::INITIALISED;
      ::std::for_each(myStartBlock->beginPreorder(), myStartBlock->endPreorder(),
          ::boost::bind(&Self::notifyInitialised, this, _1));

      // Tell any listeners
      myEventSupport.notify(event::makeStateChangedEvent(*this, oldState, myState));
      break;

    case PipelineState::RUNNING:

      ::std::for_each(myStartBlock->beginPreorder(), myStartBlock->endPreorder(),
          ::boost::bind(&Self::notifyStarting, this, _1));
      myState = PipelineState::RUNNING;
      myEventSupport.notify(event::makeStateChangedEvent(*this, oldState, myState));
      doRun();

      break;
    case PipelineState::STOPPED:
      myState = PipelineState::STOPPED;
      // Tell any listeners
      myEventSupport.notify(event::makeStateChangedEvent(*this, oldState, myState));
      break;
    case PipelineState::FINISHED:

      ::std::for_each(myStartBlock->beginPreorder(), myStartBlock->endPreorder(),
          ::boost::bind(&Self::notifyFinishing, this, _1));
      myState = PipelineState::FINISHED;
      myShared.reset(new Shared());
      ::std::for_each(myStartBlock->beginPreorder(), myStartBlock->endPreorder(),
          ::boost::bind(&Self::notifyFinished, this, _1, this));

      // Tell any listeners
      myEventSupport.notify(event::makeStateChangedEvent(*this, oldState, myState));
      break;
    }
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SerialEngine< Pipe, Shared, Global>::clear()
  {
    myStartBlock.reset();
    if(myIsRoot)
      myGlobal.reset(new Global());
    myShared.reset(new Shared());
    myState = PipelineState::UNINITIALISED;
    myChildren.clear();
    myDataStore.clear();
    myBarriers.clear();
    myMaxReleases = DEFAULT_MAX_RELEASES;
  }

template< typename Pipe, typename Shared, typename Global>
  bool
  SerialEngine< Pipe, Shared, Global>::releaseNextBarrier()
  {
    BOOST_FOREACH(BarrierType * const barrier, myBarriers)
    {
      if(barrier->hasData())
      {
        barrier->release();
        return true;
      }
    }
    return false;
  }

template< typename Pipe, typename Shared, typename Global>
  typename SerialEngine< Pipe, Shared, Global>::DataStore::iterator
  SerialEngine< Pipe, Shared, Global>::findData(const Pipe * const data)
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
  typename SerialEngine< Pipe, Shared, Global>::DataStore::const_iterator
  SerialEngine< Pipe, Shared, Global>::findData(const Pipe * const data) const
  {
    typename DataStore::const_iterator it, end = myDataStore.end();
    for(it = myDataStore.begin(); it != end; ++it)
    {
      if(&(*it) == data)
        break;
    }
    return it;
  }

}

#ifdef _MSC_VER
#  pragma warning( pop )
#endif

#endif /* SERIAL_ENGINE_DETAIL_H */
