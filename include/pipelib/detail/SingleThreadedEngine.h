/*
 * SingleThreadedEngine.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SINGLE_THREADED_ENGINE_DETAIL_H
#define SINGLE_THREADED_ENGINE_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <boost/foreach.hpp>

#include "pipelib/Barrier.h"
#include "pipelib/event/EventSupport.h"
#include "pipelib/event/PipeRunnerListener.h"
#include "pipelib/event/PipeRunnerStateChanged.h"

namespace pipelib {

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedEngine<PipelineData, SharedData, GlobalData>::run(
  StartBlockType & startBlock)
{
  SingleThreadedRunner<PipelineData, SharedData, GlobalData> runner(startBlock);
  runner.run();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename SingleThreadedEngine<PipelineData, SharedData, GlobalData>::RunnerPtr
SingleThreadedEngine<PipelineData, SharedData, GlobalData>::createRunner()
{
  typedef SingleThreadedRunner<PipelineData, SharedData, GlobalData> RunnerType;
  return myRunners.insert(
    myRunners.end(),
    new RunnerOwningPtr(new RunnerType())
  )->loan();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename SingleThreadedEngine<PipelineData, SharedData, GlobalData>::RunnerPtr
SingleThreadedEngine<PipelineData, SharedData, GlobalData>::createRunner(
  StartBlockType & pipeline)
{
  typedef SingleThreadedRunner<PipelineData, SharedData, GlobalData> RunnerType;
  return myRunners.insert(
    myRunners.end(),
    new RunnerOwningPtr(new RunnerType(pipeline))
  )->loan();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedEngine<PipelineData, SharedData, GlobalData>::loanReturned(const RunnerOwningPtr & runner)
{
  bool found = false;
  for(typename Runners::iterator it = myRunners.begin(), end = myRunners.end();
    it != end; ++it)
  {
    if(&(*it) == &runner)
    {
      myRunners.erase(it);
      found = true;
      break;
    }
  }
  PIPELIB_ASSERT(found);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::~SingleThreadedRunner()
{
  BOOST_FOREACH(typename DataStore::value_type & data, myDataStore)
  {
    delete data.first;
  }
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedRunner<PipelineData, SharedData, GlobalData>::attach(StartBlockType & pipe)
{
  if(isAttached())
    detach();

  myPipeline = &pipe;
  for(typename BlockType::PreorderIterator it = myPipeline->beginPreorder(),
    end = myPipeline->endPreorder(); it != end; ++it)
  {
    it->notifyAttached(*this);
  }
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename SingleThreadedRunner<PipelineData, SharedData, GlobalData>::StartBlockType *
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::detach()
{
  for(typename BlockType::PreorderIterator it = myPipeline->beginPreorder(),
    end = myPipeline->endPreorder(); it != end; ++it)
  {
    it->notifyDetached();
  }
  StartBlockType * const pipe = myPipeline;
  clear();
  return pipe;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
bool SingleThreadedRunner<PipelineData, SharedData, GlobalData>::isAttached() const
{
  return myPipeline != NULL;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedRunner<PipelineData, SharedData, GlobalData>::run()
{
  changeState(PipelineState::INITIALISED);
  changeState(PipelineState::RUNNING);
  changeState(PipelineState::FINISHED);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedRunner<PipelineData, SharedData, GlobalData>::run(StartBlockType & pipe)
{
  attach(pipe);
  run();
  detach();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
PipelineState::Value SingleThreadedRunner<PipelineData, SharedData, GlobalData>::getState() const
{
  return myState;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::setFinishedDataSink(
  FinishedSinkType * sink)
{
  myFinishedSink = sink;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::setDroppedDataSink(
  DroppedSinkType * sink)
{
  myDroppedSink = sink;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::addListener(
  ListenerType & listener)
{
  myRunnerEventSupport.insert(listener);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::removeListener(
  ListenerType & listener)
{
  myRunnerEventSupport.remove(listener);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
SharedData & SingleThreadedRunner<PipelineData, SharedData, GlobalData>::shared()
{
  return *mySharedData;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const SharedData &
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::shared() const
{
  return *mySharedData;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
GlobalData & SingleThreadedRunner<PipelineData, SharedData, GlobalData>::global()
{
  return *myRoot->myGlobalData;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const GlobalData &
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::global() const
{
  return *myRoot->myGlobalData;
}


template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedRunner<PipelineData, SharedData, GlobalData>::out(
  PipelineData & data, const BlockType & outBlock, const Channel channel)
{
  PipeBlockType * const inBlock = outBlock.getOutput(channel);
  if(inBlock)
    inBlock->in(data);
  else
  {
    // So this data is finished, check if we have a sink, otherwise delete
    typename DataStore::iterator it = findData(data);
    
    PIPELIB_ASSERT_MSG(it != myDataStore.end(), "Couldn't find date in data store");

    it->second.dataState = DataState::FINISHED;
    decreaseReferenceCount(it);
  }
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename SingleThreadedRunner<PipelineData, SharedData, GlobalData>::RunnerAccessType *
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::getParent()
{
  return myParent;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const typename SingleThreadedRunner<PipelineData, SharedData, GlobalData>::RunnerAccessType *
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::getParent() const
{
  return myParent;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
PipelineData & SingleThreadedRunner<PipelineData, SharedData, GlobalData>::createData()
{
  return *(myDataStore.insert(::std::make_pair(new PipelineData, Metadata())).first->first);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedRunner<PipelineData, SharedData, GlobalData>::dropData(
  PipelineData & data)
{
  // So this data is finished, check if we have a sink, otherwise delete
  typename DataStore::iterator it = findData(data);
  
  PIPELIB_ASSERT_MSG(it != myDataStore.end(), "Couldn't find date in data store");

  it->second.dataState = DataState::DROPPED;
  decreaseReferenceCount(it);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
PipelineData &
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::registerData(
  PipelineDataPtr data)
{
  return *(myDataStore.insert(::std::make_pair(data.release(), Metadata())).first->first);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
PipelineDataHandle
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::createDataHandle(
  PipelineData & data)
{
  typename DataStore::iterator it = findData(data);

  PIPELIB_ASSERT_MSG(it != myDataStore.end(), "Cannot create data handle: data not found.");

  PipelineDataHandle handle = generateHandle();
  myHandles[handle] = &data;
  increaseReferenceCount(it);

  return handle;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::releaseDataHandle(
  const PipelineDataHandle & handle)
{
  const typename HandleMap::iterator it = myHandles.find(handle);
  
  PIPELIB_ASSERT_MSG(it != myHandles.end(), "Cannot release data handle: handle not found.");

  typename DataStore::iterator storeIt = findData(*it->second);

  PIPELIB_ASSERT_MSG(storeIt != myDataStore.end(), "Cannot release data handle: data not found.");

  myHandles.erase(it);
  decreaseReferenceCount(storeIt);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
PipelineData &
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::getData(
  const PipelineDataHandle & handle)
{
  typename HandleMap::iterator it = myHandles.find(handle);
  
  PIPELIB_ASSERT_MSG(it != myHandles.end(), "Cannot get data: handle not found.");

  return *(it->second);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename SingleThreadedRunner<PipelineData, SharedData, GlobalData>::MemoryAccessBase &
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::memory()
{
  return *this;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const typename SingleThreadedRunner<PipelineData, SharedData, GlobalData>::MemoryAccessBase &
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::memory() const
{
  return *this;
}


template <typename PipelineData, typename SharedData, typename GlobalData>
typename SingleThreadedRunner<PipelineData, SharedData, GlobalData>::ChildRunnerPtr
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::createChildRunner()
{
  return myChildren.insert(
    myChildren.end(),
    new ChildRunnerOwningPtr(new SingleThreadedRunner(*myRoot, *this, myMaxReleases))
  )->loan();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename SingleThreadedRunner<PipelineData, SharedData, GlobalData>::ChildRunnerPtr
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::createChildRunner(
  StartBlockType & subpipe)
{
  return myChildren.insert(
    myChildren.end(),
    new ChildRunnerOwningPtr(new SingleThreadedRunner(*myRoot, *this, subpipe, myMaxReleases))
  )->loan();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::loanReturned(ChildRunnerOwningPtr & childRunner)
{
  bool found = false;
  for(typename ChildRunners::iterator it = myChildren.begin(), end = myChildren.end();
    it != end; ++it)
  {
    if(&(*it) == &childRunner)
    {
      myChildren.erase(it);
      found = true;
      break;
    }
  }
  PIPELIB_ASSERT(found);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedRunner<PipelineData, SharedData, GlobalData>::registerBarrier(
  BarrierType & barrier)
{
  myBarriers.insert(&barrier);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::SingleThreadedRunner(
  unsigned int maxReleases):
myGlobalData(new GlobalData()),
myRoot(this),   // The top most pipe is its own root but has no parent
myParent(NULL),
myMaxReleases(maxReleases)
{
  init();
}


template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::SingleThreadedRunner(
  StartBlockType & startBlock,
  unsigned int maxReleases):
myGlobalData(new GlobalData()),
myRoot(this),   // The top most pipe is its own root but has no parent
myParent(NULL),
myMaxReleases(maxReleases)
{
  init();
  attach(startBlock);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::SingleThreadedRunner(
  SingleThreadedRunner & root,
  SingleThreadedRunner & parent,
  const unsigned int maxReleases):
myRoot(&root),
myParent(&parent),
myMaxReleases(maxReleases)
{
  init();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::SingleThreadedRunner(
  SingleThreadedRunner & root,
  SingleThreadedRunner & parent,
  StartBlockType & pipe,
  const unsigned int maxReleases):
myRoot(&root),
myParent(&parent),
myMaxReleases(maxReleases)
{
  init();
  attach(pipe);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedRunner<PipelineData, SharedData, GlobalData>::init()
{
  myFinishedSink = NULL;
  myDroppedSink = NULL;
  myLastHandle = 0;
  clear();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedRunner<PipelineData, SharedData, GlobalData>::doRun()
{
  myPipeline->start();
  
  // Release any barriers that are waiting
  unsigned int numReleases = 0;
  while(releaseNextBarrier())
  {
    ++numReleases;
    if(numReleases >= myMaxReleases)
      break;
  }
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedRunner<PipelineData, SharedData, GlobalData>::changeState(
  const PipelineState::Value newState)
{
  const PipelineState::Value oldState = myState;
  switch(newState)
  {
  case PipelineState::INITIALISED:

    for(typename BlockType::PreorderIterator it = myPipeline->beginPreorder(),
      end = myPipeline->endPreorder(); it != end; ++it)
    {
      it->notifyInitialising(*this);
    }
    myState = PipelineState::INITIALISED;
    // Tell the blocks
    for(typename BlockType::PreorderIterator it = myPipeline->beginPreorder(),
      end = myPipeline->endPreorder(); it != end; ++it)
    {
      it->notifyInitialised();
    }
    // Tell any listeners
    myRunnerEventSupport.notify(event::makeStateChangedEvent(*this, oldState, myState));
    break;

  case PipelineState::RUNNING:

    for(typename BlockType::PreorderIterator it = myPipeline->beginPreorder(),
      end = myPipeline->endPreorder(); it != end; ++it)
    {
      it->notifyStarting();
    }
    myState = PipelineState::RUNNING;
    myRunnerEventSupport.notify(event::makeStateChangedEvent(*this, oldState, myState));
    doRun();

    break;
  case PipelineState::STOPPED:
    myState = PipelineState::STOPPED;
    // Tell any listeners
    myRunnerEventSupport.notify(event::makeStateChangedEvent(*this, oldState, myState));
    break;
  case PipelineState::FINISHED:

    for(typename BlockType::PreorderIterator it = myPipeline->beginPreorder(),
      end = myPipeline->endPreorder(); it != end; ++it)
    {
      it->notifyFinishing();
    }
    myState = PipelineState::FINISHED;
    mySharedData.reset(new SharedData());
    for(typename BlockType::PreorderIterator it = myPipeline->beginPreorder(),
      end = myPipeline->endPreorder(); it != end; ++it)
    {
      it->notifyFinished(*this);
    }
    // Tell any listeners
    myRunnerEventSupport.notify(event::makeStateChangedEvent(*this, oldState, myState));
    break;
  }
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedRunner<PipelineData, SharedData, GlobalData>::clear()
{
  myPipeline = NULL;
  if(!myRoot)
    myGlobalData.reset(new GlobalData());
  mySharedData.reset(new SharedData());
  myState = PipelineState::UNINITIALISED;
  BOOST_FOREACH(const typename DataStore::value_type & data, myDataStore)
  {
    delete data.first;
  }
  myDataStore.clear();
  myBarriers.clear();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
bool SingleThreadedRunner<PipelineData, SharedData, GlobalData>::releaseNextBarrier()
{
  bool released = false;
  BOOST_FOREACH(BarrierType * const barrier, myBarriers)
  {
    if(barrier->hasData())
    {
      barrier->release();
      released = true;
    }
  }
  return released;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename SingleThreadedRunner<PipelineData, SharedData, GlobalData>::DataStore::iterator
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::findData(const PipelineData & data)
{
  typename DataStore::iterator it, end = myDataStore.end();
  for(it = myDataStore.begin(); it != end; ++it)
  {
    if(it->first == &data)
      break;
  }
  return it;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename SingleThreadedRunner<PipelineData, SharedData, GlobalData>::DataStore::const_iterator
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::findData(const PipelineData & data) const
{
  typename DataStore::const_iterator it, end = myDataStore.end();
  for(it = myDataStore.begin(); it != end; ++it)
  {
    if(&(*it) = &data)
      break;
  }
  return it;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
PipelineDataHandle
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::generateHandle()
{
  return ++myLastHandle;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::increaseReferenceCount(
  const typename DataStore::iterator & it)
{
  ++it->second.referenceCount;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::decreaseReferenceCount(
  const typename DataStore::iterator & it)
{
  if(--it->second.referenceCount == 0)
  {
    PipelineData * tmpPtr = it->first;
    const typename DataState::Value state = it->second.dataState;
    myDataStore.erase(it);

    if(state == DataState::FINISHED && myFinishedSink)
      myFinishedSink->finished(PipelineDataPtr(tmpPtr));
    else if(state == DataState::DROPPED && myDroppedSink)
      myDroppedSink->dropped(PipelineDataPtr(tmpPtr));
    else
    {
      delete tmpPtr;
      tmpPtr = NULL;
    }
  }
}

}

#endif /* SINGLE_THREADED_ENGINE_DETAIL_H */
