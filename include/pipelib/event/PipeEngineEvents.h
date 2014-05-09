/*
 * PipeEngineEvents.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_RUNNER_EVENTS_H
#define PIPE_RUNNER_EVENTS_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/PipelineState.h"

namespace pipelib {

template< typename T, typename U, typename V>
  class PipeEngine;
template< typename T, typename U, typename V>
  class EngineAccess;

namespace event {

template< class Engine>
  class PipeEngineStateChanged
  {
  public:
    PipeEngineStateChanged(const Engine & engine, const ::pipelib::PipelineState::Value oldState,
        const ::pipelib::PipelineState::Value newState);

    const Engine &
    getEngine() const;
    ::pipelib::PipelineState::Value
    getOldState() const;
    ::pipelib::PipelineState::Value
    getNewState() const;

  private:
    const Engine & myEngine;
    const ::pipelib::PipelineState::Value myOldState;
    const ::pipelib::PipelineState::Value myNewState;
  };

template< class T, class U, class V>
  PipeEngineStateChanged< EngineAccess< T, U, V> >
  makeStateChangedEvent(const EngineAccess< T, U, V> & engine,
      const ::pipelib::PipelineState::Value oldState,
      const ::pipelib::PipelineState::Value newState)
  {
    return PipeEngineStateChanged< EngineAccess< T, U, V> >(engine, oldState, newState);
  }

template< class Engine>
  class PipeEngineDestroyed
  {
  public:
  PipeEngineDestroyed(const Engine & engine);

    const Engine &
    getEngine() const;

  private:
    const Engine & myEngine;
  };

template< class T, class U, class V>
PipeEngineDestroyed< EngineAccess< T, U, V> >
  makeDetachedEvent(const EngineAccess< T, U, V> & engine)
  {
    return PipeEngineDestroyed< EngineAccess< T, U, V> >(engine);
  }

}
}

#include "pipelib/event/detail/PipeEngineEvents.h"

#endif /* PIPE_RUNNER_EVENTS_H */
