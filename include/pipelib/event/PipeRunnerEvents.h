/*
 * PipeRunnerEvents.h
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

template <typename T, typename U, typename V>
class PipeRunner;
template <typename T, typename U, typename V>
class RunnerAccess;

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

template <class T, class U, class V>
PipeRunnerStateChanged<RunnerAccess<T, U, V> >
makeStateChangedEvent(
  const RunnerAccess<T, U, V> & runner,
  const ::pipelib::PipelineState::Value oldState,
  const ::pipelib::PipelineState::Value newState)
{
  return PipeRunnerStateChanged<RunnerAccess<T, U, V> >(runner, oldState, newState);
}

template <class Runner>
class PipeRunnerDestroyed
{
public:
  PipeRunnerDestroyed(const Runner & runner);

  const Runner & getRunner() const;

private:
  const Runner & myRunner;
};

template <class T, class U, class V>
PipeRunnerDestroyed<RunnerAccess<T, U, V> >
makeDestroyedEvent(const RunnerAccess<T, U, V> & runner)
{
  return PipeRunnerDestroyed<RunnerAccess<T, U, V> >(runner);
}

}
}

#include "pipelib/event/detail/PipeRunnerEvents.h"

#endif /* PIPE_RUNNER_EVENTS_H */
