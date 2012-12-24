/*
 * PipeRunnerStateChanged.h
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
class PipeRunner;

namespace event {


template <class Runner>
class PipeRunnerStateChanged
{
public:
  PipeRunnerStateChanged(
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
PipeRunnerStateChanged<Runner>::PipeRunnerStateChanged(
  const Runner & runner,
  const ::pipelib::PipelineState::Value oldState,
  const ::pipelib::PipelineState::Value newState):
myRunner(runner),
myOldState(oldState),
myNewState(newState)
{}

template <class Runner>
const Runner &
PipeRunnerStateChanged<Runner>::getRunner() const
{
  return myRunner;
}

template <class Runner>
::pipelib::PipelineState::Value
PipeRunnerStateChanged<Runner>::getOldState() const
{
  return myOldState;
}

template <class Runner>
::pipelib::PipelineState::Value
PipeRunnerStateChanged<Runner>::getNewState() const
{
  return myNewState;
}

template <class T, class U, class V>
PipeRunnerStateChanged<PipeRunner<T, U, V> >
makeStateChangedEvent(
  const PipeRunner<T, U, V> & runner,
  const ::pipelib::PipelineState::Value  oldState,
  const ::pipelib::PipelineState::Value  newState)
{
  return PipeRunnerStateChanged<PipeRunner<T, U, V> >(runner, oldState, newState);
}

}
}

#endif /* PIPE_STATE_CHANGED_H */
