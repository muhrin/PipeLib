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
#include "pipelib/PipeEngine.h"
#include "pipelib/PipeRunner.h"
#include "pipelib/event/EventSupport.h"

namespace pipelib {

template <typename PipelineData, typename SharedData, typename GlobalData>
class SingleThreadedEngine : public PipeEngine<PipelineData, SharedData, GlobalData>
{
  typedef PipeEngine<PipelineData, SharedData, GlobalData> Base;
public:
  typedef StartBlock<PipelineData, SharedData, GlobalData> StartBlockType;
  typedef typename Base::RunnerPtr RunnerPtr;

  virtual void run(StartBlockType & startBlock);
  virtual RunnerPtr createRunner();
  virtual RunnerPtr createRunner(StartBlockType & subpipe);

private:
  typedef LoaningPtr<typename Base::RunnerType, SingleThreadedEngine> RunnerOwningPtr;
  typedef ::boost::ptr_vector<RunnerOwningPtr> Runners;

  void loanReturned(const RunnerOwningPtr & runnerPtr);

  Runners myRunners;

  friend class LoaningPtr<typename Base::RunnerType, SingleThreadedEngine>;
};

template <typename PipelineData, typename SharedData, typename GlobalData>
class SingleThreadedRunner :
  public virtual PipeRunner<PipelineData, SharedData, GlobalData>,
  public pipelib::MemoryAccess<SharedData, GlobalData>,
  public pipelib::RunnerSetup<PipelineData, SharedData, GlobalData>,
  public virtual pipelib::RunnerAccess<PipelineData, SharedData, GlobalData>
{
  typedef Block<PipelineData, SharedData, GlobalData> BlockType;
  typedef PipeRunner<PipelineData, SharedData, GlobalData> RunnerBase;
  typedef SingleThreadedEngine<PipelineData, SharedData, GlobalData> EngineType;
  typedef pipelib::MemoryAccess<SharedData, GlobalData> MemoryAccessBase;
  typedef pipelib::RunnerSetup<PipelineData, SharedData, GlobalData> SetupBase;
  typedef LoaningPtr<RunnerBase, SingleThreadedRunner> ChildRunnerOwningPtr;

  static const unsigned int DEFAULT_MAX_RELEASES = 10000;
public:
  // Pipeline
  typedef PipeBlock<PipelineData, SharedData, GlobalData> PipeBlockType;
  typedef typename RunnerBase::StartBlockType StartBlockType;
  typedef typename SetupBase::BarrierType BarrierType;
  typedef typename SetupBase::ChildRunnerPtr ChildRunnerPtr;
  // Access
  typedef pipelib::RunnerAccess<PipelineData, SharedData, GlobalData> RunnerAccessType;
  typedef typename RunnerAccessType::PipelineDataPtr PipelineDataPtr;
  // Sinks
  typedef FinishedSink<PipelineData> FinishedSinkType;
  typedef DroppedSink<PipelineData> DroppedSinkType;
  // Event
  typedef event::PipeRunnerListener<RunnerBase> ListenerType;

  virtual ~SingleThreadedRunner();

  // From PipeRunner ////////////////////////
  virtual void attach(StartBlockType & pipe);
  virtual StartBlockType * detach();
  virtual bool isAttached() const;
  virtual void run();
  virtual void run(StartBlockType & pipe);
  virtual PipelineState::Value getState() const;
  virtual SingleThreadedRunner * getParent();
  virtual const SingleThreadedRunner * getParent() const;
  // Sinks
  virtual void setFinishedDataSink(FinishedSinkType * sink);
  virtual void setDroppedDataSink(DroppedSinkType * sink);
  // Event
  virtual void addListener(ListenerType & listener);
  virtual void removeListener(ListenerType & listener);
  // End from PipeRunner ////////////////////

  // From MemoryAccess ////////////////////////
  virtual SharedData & shared();
  virtual const SharedData & shared() const;
  virtual GlobalData & global();
  virtual const GlobalData & global() const;
  // End from MemoryAccess ///////////////////

  // From RunnerAccess /////////////////////////
  // Pipeline methods
  virtual void out(PipelineData & data, const BlockType & outBlock, const Channel channel);
  virtual RunnerAccessType * getParentAccess();
  virtual const RunnerAccessType * getParentAccess() const;
  // Data methods
  virtual PipelineData & createData();
  virtual void dropData(PipelineData & toDrop);
  virtual PipelineData & registerData(PipelineDataPtr data);
  virtual PipelineDataHandle createDataHandle(PipelineData & data);
  virtual void releaseDataHandle(const PipelineDataHandle & handle);
  virtual PipelineData & getData(const PipelineDataHandle & handle);
  // Memory methods 
  virtual MemoryAccessBase & memory();
  virtual const MemoryAccessBase & memory() const;
  // End from RunnerAccess //////////////////////

  // From RunnerSetup ////////////////////////////
  virtual ChildRunnerPtr createChildRunner();
  virtual ChildRunnerPtr createChildRunner(StartBlockType & subpipe);
  virtual void registerBarrier(BarrierType & barrier);
  // End from RunnerSetup /////////////////////////

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
  typedef event::EventSupport<ListenerType> RunnerEventSupport;
  
  SingleThreadedRunner(unsigned int maxReleases = DEFAULT_MAX_RELEASES);
  SingleThreadedRunner(
    StartBlockType & startBlock,
    unsigned int maxReleases = DEFAULT_MAX_RELEASES
  );
  SingleThreadedRunner(
    SingleThreadedRunner & root,
    SingleThreadedRunner & parent,
    const unsigned int maxReleases);
  SingleThreadedRunner(
    SingleThreadedRunner & root,
    SingleThreadedRunner & parent,
    StartBlockType & subpipe,
    const unsigned int maxReleases);

  void init();

  void doRun();
  void changeState(const PipelineState::Value newState);
  void clear();
  bool releaseNextBarrier();
  typename DataStore::iterator findData(const PipelineData & data);
  typename DataStore::const_iterator findData(const PipelineData & data) const;
  PipelineDataHandle generateHandle();
  void increaseReferenceCount(const typename DataStore::iterator & it);
  void decreaseReferenceCount(const typename DataStore::iterator & it);

  void loanReturned(ChildRunnerOwningPtr & childRunner);

  // Parent/Children
  SingleThreadedRunner * const myRoot;
  SingleThreadedRunner * const myParent;
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

  // Event
  RunnerEventSupport myRunnerEventSupport;

  friend class SingleThreadedEngine<PipelineData, SharedData, GlobalData>;
  friend class LoaningPtr<RunnerBase, SingleThreadedRunner>;
};

}

#include "pipelib/detail/SingleThreadedEngine.h"

#endif /* SINGLE_THREADED_ENGINE_H */
