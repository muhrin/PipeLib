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

namespace pipelib {

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedEngine<PipelineData, SharedData, GlobalData>::run(
  StartBlockType & startBlock)
{
  SingleThreadedRunner<PipelineData, SharedData, GlobalData> runner(startBlock);
  runner.run();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::SingleThreadedRunner(
  StartBlockType & startBlock,
  unsigned int maxReleases):
myRoot(NULL),
myMaxReleases(maxReleases),
myFinishedSink(NULL),
myDroppedSink(NULL),
myLastHandle(0)
{
  clear();
  attach(startBlock);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::~SingleThreadedRunner()
{
  BOOST_FOREACH(DataStore::value_type & data, myDataStore)
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
  for(BlockType::PreorderIterator it = myPipeline->beginPreorder(),
    end = myPipeline->endPreorder(); it != end; ++it)
  {
    it->notifyAttached(*this);
  }
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename SingleThreadedRunner<PipelineData, SharedData, GlobalData>::StartBlockType *
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::detach()
{
  for(BlockType::PreorderIterator it = myPipeline->beginPreorder(),
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
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::setFinishedDataSink(
  FinishedSink * sink)
{
  myFinishedSink = sink;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::setDroppedDataSink(
  DroppedSink * sink)
{
  myDroppedSink = sink;
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
  if(myRoot)
    return myRoot->global();
  else
    return *myGlobalData;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const GlobalData &
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::global() const
{
  if(myRoot)
    return myRoot->global();
  else
    return *myGlobalData;
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
    DataStore::iterator it = findData(data);
    
    PIPELIB_ASSERT_MSG(it != myDataStore.end(), "Couldn't find date in data store");

    it->second.dataState = DataState::FINISHED;
    decreaseReferenceCount(it);
  }
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
  DataStore::iterator it = findData(data);
  
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
  DataStore::iterator it = findData(data);

  PIPELIB_ASSERT_MSG(it != myDataStore.end(), "Cannot create data handle: data not found.");

  PipelineDataHandle handle = generateHandle();
  myHandles[handle] = &data;
  increaseReferenceCount(it);

  return handle;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::releaseDataHandle(
  PipelineDataHandle & handle)
{
  const HandleMap::iterator it = myHandles.find(handle);
  
  PIPELIB_ASSERT_MSG(it != myHandles.end(), "Cannot release data handle: handle not found.");

  DataStore::iterator storeIt = findData(*it->second);

  PIPELIB_ASSERT_MSG(storeIt != myDataStore.end(), "Cannot release data handle: data not found.");

  myHandles.erase(it);
  decreaseReferenceCount(storeIt);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
PipelineData &
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::getData(
  PipelineDataHandle & handle)
{
  HandleMap::iterator it = myHandles.find(handle);
  
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
    new ChildRunnerOwningPtr(new SingleThreadedRunner(*this), this)
  )->loan();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename SingleThreadedRunner<PipelineData, SharedData, GlobalData>::ChildRunnerPtr
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::createChildRunner(
  StartBlockType & subpipe)
{
  return myChildren.insert(
    myChildren.end(),
    new ChildRunnerOwningPtr(new SingleThreadedRunner(*this, subpipe), this)
  )->loan();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::loanReturned(ChildRunnerOwningPtr & childRunner)
{
  bool found = false;
  for(ChildRunners::iterator it = myChildren.begin(), end = myChildren.end();
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
  SingleThreadedRunner & rootRunner):
myRoot(&rootRunner)
{
  clear();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedRunner<PipelineData, SharedData, GlobalData>::SingleThreadedRunner(
  SingleThreadedRunner & rootRunner,
  StartBlockType & pipe):
myRoot(&rootRunner)
{
  clear();
  attach(pipe);
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
  switch(newState)
  {
  case PipelineState::INITIALISED:

    for(BlockType::PreorderIterator it = myPipeline->beginPreorder(),
      end = myPipeline->endPreorder(); it != end; ++it)
    {
      it->notifyInitialising(*this);
    }
    myState = PipelineState::INITIALISED;
    for(BlockType::PreorderIterator it = myPipeline->beginPreorder(),
      end = myPipeline->endPreorder(); it != end; ++it)
    {
      it->notifyInitialised();
    }
    break;

  case PipelineState::RUNNING:

    for(BlockType::PreorderIterator it = myPipeline->beginPreorder(),
      end = myPipeline->endPreorder(); it != end; ++it)
    {
      it->notifyStarting();
    }
    myState = PipelineState::RUNNING;
    doRun();

    break;
  case PipelineState::STOPPED:
    myState = PipelineState::STOPPED;
    break;
  case PipelineState::FINISHED:

    for(BlockType::PreorderIterator it = myPipeline->beginPreorder(),
      end = myPipeline->endPreorder(); it != end; ++it)
    {
      it->notifyFinishing();
    }
    myState = PipelineState::FINISHED;
    for(BlockType::PreorderIterator it = myPipeline->beginPreorder(),
      end = myPipeline->endPreorder(); it != end; ++it)
    {
      it->notifyFinished(*this);
    }
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
  BOOST_FOREACH(const DataStore::value_type & data, myDataStore)
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
  DataStore::iterator it, end = myDataStore.end();
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
  DataStore::const_iterator it;
  for(it = myDataStore.begin(), end = myDataStore.end();
    it != end; ++it)
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
    const DataState::Value state = it->second.dataState;
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