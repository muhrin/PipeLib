/*
 * PipeEngineStateChanged.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_RUNNER_STATE_CHANGED_H
#define PIPE_RUNNER_STATE_CHANGED_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/PipelineState.h"

namespace pipelib {

template <typename T, typename U, typename V>
class PipeEngine;

namespace event {


template <class Runner>
class PipeEngineStateChanged
{
public:
  PipeEngineStateChanged(
    const Runner & runner,
    const ::pipelib::PipelineState::Value  oldState,
    const ::pipelib::PipelineState::Value  newState);

  const Runner & getRunner() const;
  ::pipelib::PipelineState::Value getOldState() const;
  ::pipelib::PipelineState::Value getNewState() const;
  

private:
  const Runner &                          myRunner;
  const ::pipelib::PipelineState::Value   myOldState;
  const ::pipelib::PipelineState::Value   myNewState;

};

// IMPLEMENTATION ///////////////////////////////////////////////////

template <class Runner>
PipeEngineStateChanged<Runner>::PipeEngineStateChanged(
  const Runner & runner,
  const ::pipelib::PipelineState::Value oldState,
  const ::pipelib::PipelineState::Value newState):
myRunner(runner),
myOldState(oldState),
myNewState(newState)
{}

template <class Runner>
const Runner &
PipeEngineStateChanged<Runner>::getRunner() const
{
  return myRunner;
}

template <class Runner>
::pipelib::PipelineState::Value
PipeEngineStateChanged<Runner>::getOldState() const
{
  return myOldState;
}

template <class Runner>
::pipelib::PipelineState::Value
PipeEngineStateChanged<Runner>::getNewState() const
{
  return myNewState;
}

template <class T, class U, class V>
PipeEngineStateChanged<PipeEngine<T, U, V> >
makeStateChangedEvent(
  const PipeEngine<T, U, V> & runner,
  const ::pipelib::PipelineState::Value  oldState,
  const ::pipelib::PipelineState::Value  newState)
{
  return PipeEngineStateChanged<PipeEngine<T, U, V> >(runner, oldState, newState);
}

}
}

#endif /* PIPE_STATE_CHANGED_H */
