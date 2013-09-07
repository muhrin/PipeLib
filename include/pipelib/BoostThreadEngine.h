/*
 * BoostThreadEngine.h
 *
 *
 *  Created on: Sep 6, 2013
 *      Author: Martin Uhrin
 */

#ifndef BOOST_THREAD_ENGINE_H
#define BOOST_THREAD_ENGINE_H

#include "pipelib/Pipeline.h"

#ifdef PIPELIB_USE_BOOST_THREAD

#include <boost/asio.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "pipelib/PipeEngine.h"
#include "pipelib/event/EventSupport.h"
#include "pipelib/PipelineState.h"

namespace pipelib {

template< typename Pipe, typename Shared, typename Global>
  class BoostThreadEngine : public virtual PipeEngine< Pipe, Shared, Global>,
      public pipelib::EngineSetup< Pipe, Shared, Global>,
      public virtual pipelib::EngineAccess< Pipe, Shared, Global>
  {
    typedef Block< Pipe, Shared, Global> BlockType;
    typedef typename BlockType::HandleType BlockHandleType;
    typedef pipelib::EngineSetup< Pipe, Shared, Global> SetupBase;
    typedef PipeEngine< Pipe, Shared, Global> Base;
    typedef BoostThreadEngine< Pipe, Shared, Global> Self;

    static const unsigned int DEFAULT_MAX_RELEASES = 10000;
  public:
    // Pipeline
    typedef PipeBlock< Pipe, Shared, Global> PipeBlockType;
    typedef StartBlock< Pipe, Shared, Global> StartBlockType;
    typedef typename SetupBase::BarrierType BarrierType;
    // Access
    typedef pipelib::EngineAccess< Pipe, Shared, Global> EngineAccessType;
    typedef typename EngineAccessType::PipePtr PipePtr;
    // Sinks
    typedef FinishedSink< Pipe> FinishedSinkType;
    typedef DroppedSink< Pipe> DroppedSinkType;
    // Event
    typedef typename EngineAccessType::ListenerType ListenerType;

    BoostThreadEngine(const size_t numThreads);
    BoostThreadEngine(const size_t numThreads, BlockHandleType & startBlock);
    virtual ~BoostThreadEngine();

    // From PipeEngine ////////////////////////
    virtual void
    attach(BlockHandleType & startBlock);
    virtual bool
    detach();
    virtual bool
    isAttached() const;
    virtual void
    run();
    virtual void
    run(BlockHandleType & startBlock);
    virtual PipelineState::Value
    getState() const;
    virtual const BlockHandleType &
    getStartBlock() const;
    // Sinks
    virtual void
    setFinishedDataSink(FinishedSinkType * sink);
    virtual void
    setDroppedDataSink(DroppedSinkType * sink);
    // Event
    virtual void
    addListener(ListenerType & listener);
    virtual void
    removeListener(ListenerType & listener);
    // End from PipeEngine ////////////////////

    virtual Shared &
    sharedData();
    virtual const Shared &
    sharedData() const;
    virtual Global &
    globalData();
    virtual const Global &
    globalData() const;

    // From EngineAccess /////////////////////////
    // Pipeline methods
    virtual void
    out(Pipe * data, const BlockType & outBlock, const Channel channel);
    // Data methods
    virtual Pipe *
    createData();
    virtual void
    dropData(Pipe * toDrop);
    virtual Pipe *
    registerData(PipePtr data);
    // End from EngineAccess //////////////////////

    // From EngineSetup ////////////////////////////
    virtual Base *
    createEngine();
    virtual void
    releaseEngine(Base * engine);

    virtual void
    registerBarrier(BarrierType * const barrier);
    // End from EngineSetup /////////////////////////

  private:
    typedef ::boost::shared_ptr< Global> GlobalPtr;
    typedef ::boost::scoped_ptr< Shared> SharedPtr;
    typedef ::boost::ptr_vector< Base> ChildEngines;
    typedef ::boost::ptr_vector< Pipe> DataStore;
    typedef ::std::vector< BarrierType *> Barriers;
    typedef event::EventSupport< ListenerType> EngineEventSupport;

    BoostThreadEngine(Self * root);
    BoostThreadEngine(Self * root, BlockHandleType & subpipe);

    void
    init();

    void
    doRun();
    void
    changeState(const PipelineState::Value newState);
    void
    clear();
    bool
    releaseNextBarrier();
    typename DataStore::iterator
    findData(const Pipe * const data);
    typename DataStore::const_iterator
    findData(const Pipe * const data) const;
    bool
    isRoot() const;

    void runTillFinished() const;

    void incrementNumRunning();
    void decrementNumRunning();

    template<typename Task>
    void
    postTask(Task task);
    void
    startTask(StartBlockType * const startBlock);
    void
    outTask(Pipe * const data, PipeBlockType * const inBlock);
    void
    releaseBarrierTask(BarrierType * const barrier);

    // Parent/Children
    Self * const myRoot;
    ChildEngines myChildren;

    // Barriers
    unsigned int myMaxReleases;
    Barriers myBarriers;

    // Data
    DataStore myDataStore;
    GlobalPtr myGlobal;
    SharedPtr myShared;

    BlockHandleType myStartBlock;

    PipelineState::Value myState;

    // Sinks
    FinishedSinkType * myFinishedSink;
    DroppedSinkType * myDroppedSink;

    // Event
    EngineEventSupport myEventSupport;

    const size_t myNumThreads;
    size_t myNumRunning;
    ::boost::mutex myNumRunningMutex;

    // Threading stuff
    struct Threading
    {
      ::boost::thread_group threads;
      ::boost::asio::io_service threadService;
    };

    ::boost::scoped_ptr<Threading> myThreading;
  };

}

#include "pipelib/detail/BoostThreadEngine.h"

#endif /* PIPELIB_USE_BOOST_THREAD */
#endif /* BOOST_THREAD_ENGINE_H */
