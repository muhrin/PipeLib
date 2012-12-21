/*
 * SingleThreadedEngine.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SINGLE_THREADED_ENGINE_H
#define SINGLE_THREADED_ENGINE_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"

#include <map>
#include <set>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "pipelib/PipelineState.h"
#include "pipelib/PipeRunner.h"

namespace pipelib {

template <typename PipelineData, typename SharedData, typename GlobalData>
class SingleThreadedEngine
{
public:

  typedef Block<PipelineData, SharedData, GlobalData> BlockType;
  typedef PipeBlock<PipelineData, SharedData, GlobalData> PipeBlockType;
  typedef StartBlock<PipelineData, SharedData, GlobalData> StartBlockType;

  virtual void run(StartBlockType & startBlock);

};

template <typename PipelineData, typename SharedData, typename GlobalData>
class SingleThreadedRunner :
  public PipeRunner<PipelineData, SharedData, GlobalData>,
  public pipelib::MemoryAccess<SharedData, GlobalData>,
  public pipelib::RunnerSetup<PipelineData, SharedData, GlobalData>,
  public pipelib::RunnerAccess<PipelineData, SharedData, GlobalData>
{
  typedef Block<PipelineData, SharedData, GlobalData> BlockType;
  typedef PipeRunner<PipelineData, SharedData, GlobalData> RunnerBase;
  typedef SingleThreadedEngine<PipelineData, SharedData, GlobalData> EngineType;
  typedef pipelib::RunnerAccess<PipelineData, SharedData, GlobalData> AccessBase;
  typedef pipelib::MemoryAccess<SharedData, GlobalData> MemoryAccessBase;
  typedef pipelib::RunnerSetup<PipelineData, SharedData, GlobalData> SetupBase;
  typedef OwningPtr<RunnerBase, SingleThreadedRunner> ChildRunnerOwningPtr;

  static const unsigned int DEFAULT_MAX_RELEASES = 10000;
public:

  typedef PipeBlock<PipelineData, SharedData, GlobalData> PipeBlockType;
  typedef typename RunnerBase::StartBlockType StartBlockType;
  typedef typename SetupBase::BarrierType BarrierType;
  typedef typename SetupBase::ChildRunnerPtr ChildRunnerPtr;
  typedef typename AccessBase::PipelineDataPtr PipelineDataPtr;
  typedef FinishedSink<PipelineData> FinishedSinkType;
  typedef DroppedSink<PipelineData> DroppedSinkType;

  SingleThreadedRunner(
    StartBlockType & startBlock,
    unsigned int maxReleases = DEFAULT_MAX_RELEASES
  );
  virtual ~SingleThreadedRunner();

  // From PipeRunner ////////////////////////
  virtual void attach(StartBlockType & pipe);
  virtual StartBlockType * detach();
  virtual bool isAttached() const;
  virtual void run();
  virtual void run(StartBlockType & pipe);
  virtual void setFinishedDataSink(FinishedSinkType * sink);
  virtual void setDroppedDataSink(DroppedSinkType * sink);
  // End from PipeRunner ////////////////////

  // From MemoryAccess ////////////////////////
  virtual SharedData & shared();
  virtual const SharedData & shared() const;
  virtual GlobalData & global();
  virtual const GlobalData & global() const;
  // End from MemoryAccess ///////////////////

  // From RunnerAccess /////////////////////////
  virtual void out(PipelineData & data, const BlockType & outBlock, const Channel channel);
  virtual PipelineData & createData();
  virtual void dropData(PipelineData & toDrop);
  virtual PipelineData & registerData(PipelineDataPtr data);
  virtual PipelineDataHandle createDataHandle(PipelineData & data);
  virtual void releaseDataHandle(PipelineDataHandle & handle);
  virtual PipelineData & getData(PipelineDataHandle & handle);
  virtual MemoryAccessBase & memory();
  virtual const MemoryAccessBase & memory() const;
  // End from RunnerAccess //////////////////////

  // From RunnerSetup ////////////////////////////
  virtual ChildRunnerPtr createChildRunner();
  virtual ChildRunnerPtr createChildRunner(StartBlockType & subpipe);
  virtual void registerBarrier(BarrierType & barrier);
  // End from RunnerSetup /////////////////////////

  void loanReturned(ChildRunnerOwningPtr & childRunner);

private:

  struct DataState
  {
    enum Value { FRESH, FINISHED, DROPPED };
  };

  struct Metadata
  {
    Metadata(): dataState(DataState::FRESH), referenceCount(1) {}
    typename DataState::Value dataState;
    unsigned int referenceCount;
  };

  typedef ::boost::shared_ptr<GlobalData> GlobalDataPtr;
  typedef ::boost::scoped_ptr<SharedData> SharedDataPtr;
  typedef ::boost::ptr_vector<ChildRunnerOwningPtr> ChildRunners;
  typedef ::std::map<PipelineData *, Metadata> DataStore;
  typedef ::std::set<BarrierType *> Barriers;
  typedef ::std::map<PipelineDataHandle, PipelineData *> HandleMap;
  
  SingleThreadedRunner(SingleThreadedRunner & rootRunner);
  SingleThreadedRunner(SingleThreadedRunner & rootRunner, StartBlockType & subpipe);

  void doRun();
  void changeState(const PipelineState::Value newState);
  void clear();
  bool releaseNextBarrier();
  typename DataStore::iterator findData(const PipelineData & data);
  typename DataStore::const_iterator findData(const PipelineData & data) const;
  PipelineDataHandle generateHandle();
  void increaseReferenceCount(const typename DataStore::iterator & it);
  void decreaseReferenceCount(const typename DataStore::iterator & it);

  // Parent/Children
  SingleThreadedRunner * const myRoot;
  ChildRunners myChildren;

  // Barriers
  unsigned int myMaxReleases;
  Barriers myBarriers;

  // Data
  DataStore myDataStore;
  GlobalDataPtr myGlobalData;
  SharedDataPtr mySharedData;
  HandleMap myHandles;
  PipelineDataHandle myLastHandle;

  // Pipeline
  StartBlockType * myPipeline;

  // State
  PipelineState::Value myState;

  // Sinks
  FinishedSinkType * myFinishedSink;
  DroppedSinkType * myDroppedSink;
};

}

#include "pipelib/detail/SingleThreadedEngine.h"

#endif /* SINGLE_THREADED_ENGINE_H */
