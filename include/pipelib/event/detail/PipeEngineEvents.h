/*
 * PipeEngineEvents.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_RUNNER_EVENTS_DETAIL_H
#define PIPE_RUNNER_EVENTS_DETAIL_H

namespace pipelib {
namespace event {

template< class Engine>
  PipeEngineStateChanged< Engine>::PipeEngineStateChanged(const Engine & engine,
      const ::pipelib::PipelineState::Value oldState,
      const ::pipelib::PipelineState::Value newState) :
      myEngine(engine), myOldState(oldState), myNewState(newState)
  {
  }

template< class Engine>
  const Engine &
  PipeEngineStateChanged< Engine>::getEngine() const
  {
    return myEngine;
  }

template< class Engine>
  ::pipelib::PipelineState::Value
  PipeEngineStateChanged< Engine>::getOldState() const
  {
    return myOldState;
  }

template< class Engine>
  ::pipelib::PipelineState::Value
  PipeEngineStateChanged< Engine>::getNewState() const
  {
    return myNewState;
  }

template< class Engine>
PipeEngineDestroyed< Engine>::PipeEngineDestroyed(const Engine & engine) :
      myEngine(engine)
  {
  }

template< class Engine>
  const Engine &
  PipeEngineDestroyed< Engine>::getEngine() const
  {
    return myEngine;
  }

}
}

#endif /* PIPE_RUNNER_EVENTS_DETAIL_H */
