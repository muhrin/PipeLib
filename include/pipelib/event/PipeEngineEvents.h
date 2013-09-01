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

template< class Runner>
  class PipeEngineStateChanged
  {
  public:
    PipeEngineStateChanged(const Runner & runner, const ::pipelib::PipelineState::Value oldState,
        const ::pipelib::PipelineState::Value newState);

    const Runner &
    getRunner() const;
    ::pipelib::PipelineState::Value
    getOldState() const;
    ::pipelib::PipelineState::Value
    getNewState() const;

  private:
    const Runner & myRunner;
    const ::pipelib::PipelineState::Value myOldState;
    const ::pipelib::PipelineState::Value myNewState;
  };

template< class T, class U, class V>
  PipeEngineStateChanged< EngineAccess< T, U, V> >
  makeStateChangedEvent(const EngineAccess< T, U, V> & runner,
      const ::pipelib::PipelineState::Value oldState,
      const ::pipelib::PipelineState::Value newState)
  {
    return PipeEngineStateChanged< EngineAccess< T, U, V> >(runner, oldState, newState);
  }

template< class Runner>
  class PipeEngineDestroyed
  {
  public:
    PipeEngineDestroyed(const Runner & runner);

    const Runner &
    getRunner() const;

  private:
    const Runner & myRunner;
  };

template< class T, class U, class V>
  PipeEngineDestroyed< EngineAccess< T, U, V> >
  makeDestroyedEvent(const EngineAccess< T, U, V> & runner)
  {
    return PipeEngineDestroyed< EngineAccess< T, U, V> >(runner);
  }

}
}

#include "pipelib/event/detail/PipeEngineEvents.h"

#endif /* PIPE_RUNNER_EVENTS_H */
