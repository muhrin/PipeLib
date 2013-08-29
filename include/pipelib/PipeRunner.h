/*
 * PipeRunner.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_RUNNER_H
#define PIPE_RUNNER_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"
#include "pipelib/LoaningPtr.h"
#include "pipelib/Sinks.h"
#include "pipelib/PipelineState.h"

namespace pipelib {

// FORWARD DECLARES //////////////////////////////////////////
template <typename PipelineData, typename SharedData, typename GlobalData>
class Block;

template <typename PipelineData, typename SharedData, typename GlobalData>
class Barrier;

template <typename PipelineData, typename SharedData, typename GlobalData>
class Pipe;

template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeBlock;

namespace event {
template <typename Runner>
class PipeRunnerListener;
}

/**
/*  Methods needed to access a runner's memory.
/*
/**/
template <typename SharedData, typename GlobalData>
class MemoryAccess
{
public:
  virtual ~MemoryAccess() {}

  virtual SharedData & shared() = 0;
  virtual const SharedData & shared() const = 0;
  virtual GlobalData & global() = 0;
  virtual const GlobalData & global() const = 0;
};

/**
/* Methods needed by a blocks whilst the pipeline is running.
/*
/**/
template <typename PipelineData, typename SharedData, typename GlobalData>
class RunnerAccess
{
public:
  typedef Block<PipelineData, SharedData, GlobalData> BlockType;
  typedef typename UniquePtr<PipelineData>::Type PipelineDataPtr;
  typedef event::PipeRunnerListener<RunnerAccess> ListenerType;

  virtual ~RunnerAccess() {}

  // Pipeline methods
  virtual void out(PipelineData & data, const BlockType & outBlock, const Channel channel) = 0;
  virtual RunnerAccess * getParentAccess() = 0;
  virtual const RunnerAccess * getParentAccess() const = 0;
  virtual PipelineState::Value getState() const = 0;

  // Pipeline data methods
  virtual PipelineData & createData() = 0;
  virtual void dropData(PipelineData & toDrop) = 0;
  virtual PipelineData & registerData(PipelineDataPtr data) = 0;
  virtual PipelineDataHandle createDataHandle(PipelineData & data) = 0;
  virtual void releaseDataHandle(const PipelineDataHandle & handle) = 0;
  virtual PipelineData & getData(const PipelineDataHandle & handle) = 0;

  // Memory methods
  virtual MemoryAccess<SharedData, GlobalData> & memory() = 0;
  virtual const MemoryAccess<SharedData, GlobalData> & memory() const = 0;

  // Event
  virtual void addListener(ListenerType & listener) = 0;
  virtual void removeListener(ListenerType & listener) = 0;
};

template <typename PipelineData, typename SharedData, typename GlobalData>
class RunnerSetup;

template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeRunner
{
public:
  typedef Pipe<PipelineData, SharedData, GlobalData> PipeType;

  virtual ~PipeRunner() {}

  virtual void attach(PipeType & pipe) = 0;
  virtual void detach() = 0;
  virtual bool isAttached() const = 0;
  virtual void run() = 0;
  virtual void run(PipeType & pipe) = 0;
  virtual PipelineState::Value getState() const = 0;
  virtual PipeRunner * getParent() = 0;
  virtual const PipeRunner * getParent() const = 0;

  // Sinks
  virtual void setFinishedDataSink(FinishedSink<PipelineData> * sink) = 0;
  virtual void setDroppedDataSink(DroppedSink<PipelineData> * sink) = 0;

  // Memory methods
  virtual MemoryAccess<SharedData, GlobalData> & memory() = 0;
  virtual const MemoryAccess<SharedData, GlobalData> & memory() const = 0;

protected:

  typedef RunnerAccess<PipelineData, SharedData, GlobalData> RunnerAccessType;
  typedef RunnerSetup<PipelineData, SharedData, GlobalData> RunnerSetupType;

  void notifyAttached(PipeType & pipe, RunnerSetupType & setup)
  { pipe.notifyAttached(setup); }
  void notifyInitialising(PipeType & pipe, RunnerAccessType & access)
  { pipe.notifyInitialising(access); }
  void notifyInitialised(PipeType & pipe)
  { pipe.notifyInitialised(); }
  void notifyStarting(PipeType & pipe)
  { pipe.notifyStarting(); }
  void notifyFinishing(PipeType & pipe)
  { pipe.notifyFinishing(); }
  void notifyFinished(PipeType & pipe, RunnerAccessType & access)
  { pipe.notifyFinished(access); }
  void notifyDetached(PipeType & pipe)
  { pipe.notifyDetached(); }
};

/**
/* Class template that defines the methods needed by blocks
/* when the runner is being attached to the pipe to perform
/* any required set up.
/**/
template <typename PipelineData, typename SharedData, typename GlobalData>
class RunnerSetup
{
public:
  typedef Pipe<PipelineData, SharedData, GlobalData> PipeType;
  typedef PipeRunner<PipelineData, SharedData, GlobalData> RunnerType;
  typedef Barrier<PipelineData, SharedData, GlobalData> BarrierType;
  typedef LoanPtr<RunnerType> ChildRunnerPtr;

  virtual ~RunnerSetup() {}

  virtual ChildRunnerPtr createChildRunner() = 0;
  virtual ChildRunnerPtr createChildRunner(PipeType & subpipe) = 0;
  virtual void registerBarrier(BarrierType & barrier) = 0;
};

template <class T>
struct PipeRunnerTypes
{
  typedef PipeRunner<
    typename T::PipelineDataType,
    typename T::SharedDataType,
    typename T::GlobalDataType
  > Runner;
  typedef RunnerSetup<
    typename T::PipelineDataType,
    typename T::SharedDataType,
    typename T::GlobalDataType
  > Setup;
  typedef RunnerAccess<
    typename T::PipelineDataType,
    typename T::SharedDataType,
    typename T::GlobalDataType
  > Access;
};

}

#endif /* PIPE_RUNNER_H */
