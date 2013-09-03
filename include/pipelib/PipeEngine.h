/*
 * PipeEngine.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_ENGINE_H
#define PIPE_ENGINE_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"

#include <vector>

#include "pipelib/BlockHandle.h"
#include "pipelib/Sinks.h"
#include "pipelib/PipelineState.h"

namespace pipelib {

// FORWARD DECLARES //////////////////////////////////////////
template <typename Pipe, typename Shared, typename Global>
class Block;

template <typename Pipe, typename Shared, typename Global>
class Barrier;

namespace event {
template <typename Engine>
class PipeEngineListener;
}

/**
/*  Methods needed to access a engine's data.
/*
/**/
template <typename Shared, typename Global>
class DataAccess
{
public:
  virtual ~DataAccess() {}

  virtual Shared & sharedData() = 0;
  virtual const Shared & sharedData() const = 0;
  virtual Global & globalData() = 0;
  virtual const Global & globalData() const = 0;
};

/**
/* Methods needed by a blocks while the pipeline is running.
/*
/**/
template <typename Pipe, typename Shared, typename Global>
class EngineAccess : public DataAccess<Shared, Global>
{
public:
  typedef Block<Pipe, Shared, Global> BlockType;
  typedef typename UniquePtr<Pipe>::Type PipePtr;
  typedef event::PipeEngineListener<EngineAccess> ListenerType;

  virtual ~EngineAccess() {}

  // Pipeline methods
  virtual void out(Pipe * data, const BlockType & outBlock, const Channel channel) = 0;
  virtual PipelineState::Value getState() const = 0;

  // Pipeline data methods
  virtual Pipe * createData() = 0;
  virtual void dropData(Pipe * data) = 0;
  virtual Pipe * registerData(PipePtr data) = 0;

  // Event
  virtual void addListener(ListenerType & listener) = 0;
  virtual void removeListener(ListenerType & listener) = 0;
};

template <typename Pipe, typename Shared, typename Global>
class EngineSetup;

template <typename Pipe, typename Shared, typename Global>
class PipeEngine : public DataAccess<Shared, Global>
{
public:
  typedef Block<Pipe, Shared, Global> BlockType;
  typedef typename BlockType::HandleType BlockHandleType;
  typedef EngineAccess<Pipe, Shared, Global> AccessType;
  typedef EngineSetup<Pipe, Shared, Global> SetupType;

  virtual ~PipeEngine() {}

  virtual void attach(BlockHandleType & startBlock) = 0;
  virtual bool detach() = 0;
  virtual bool isAttached() const = 0;
  virtual void run() = 0;
  virtual void run(BlockHandleType & startBlock) = 0;
  virtual PipelineState::Value getState() const = 0;
  virtual const BlockHandleType & getStartBlock() const = 0;

  // Sinks
  virtual void setFinishedDataSink(FinishedSink<Pipe> * sink) = 0;
  virtual void setDroppedDataSink(DroppedSink<Pipe> * sink) = 0;

protected:
  typedef Barrier<Pipe, Shared, Global> BarrierType;
  typedef ::std::vector<BarrierType *> Barriers;

  void sortBarriers(Barriers * const barriers) const;

  void notifyAttached(const BlockHandleType & block, SetupType * const setup)
  { block->notifyAttached(setup); }
  void notifyInitialising(const BlockHandleType & block, AccessType * const access)
  { block->notifyInitialising(access); }
  void notifyInitialised(const BlockHandleType & block)
  { block->notifyInitialised(); }
  void notifyStarting(const BlockHandleType & block)
  { block->notifyStarting(); }
  void notifyFinishing(const BlockHandleType & block)
  { block->notifyFinishing(); }
  void notifyFinished(const BlockHandleType & block, AccessType * const access)
  { block->notifyFinished(access); }
  void notifyDetached(const BlockHandleType & block)
  { block->notifyDetached(); }
};

/**
/* Class template that defines the methods needed by blocks
/* when the engine is being attached to the pipe to perform
/* any required set up.
/**/
template <typename Pipe, typename Shared, typename Global>
class EngineSetup
{
public:
  typedef typename BlockHandle<Pipe, Shared, Global>::Type BlockHandleType;
  typedef PipeEngine<Pipe, Shared, Global> EngineType;
  typedef Barrier<Pipe, Shared, Global> BarrierType;

  virtual ~EngineSetup() {}

  virtual EngineType * createEngine() = 0;
  virtual void releaseEngine(EngineType * engine) = 0;

  virtual void registerBarrier(BarrierType * const barrier) = 0;
};

template <class T>
struct PipeEngineTypes
{
  typedef PipeEngine<
    typename T::PipeType,
    typename T::SharedType,
    typename T::GlobalType
  > Engine;
  typedef EngineSetup<
    typename T::PipeType,
    typename T::SharedType,
    typename T::GlobalType
  > Setup;
  typedef EngineAccess<
    typename T::PipeType,
    typename T::SharedType,
    typename T::GlobalType
  > Access;
};

}

#include "pipelib/detail/PipeEngine.h"


#endif /* PIPE_ENGINE_H */
