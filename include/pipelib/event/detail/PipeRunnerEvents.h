/*
 * PipeRunnerEvents.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_RUNNER_EVENTS_DETAIL_H
#define PIPE_RUNNER_EVENTS_DETAIL_H

namespace pipelib {
namespace event {

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

template <class Runner>
PipeRunnerDestroyed<Runner>::PipeRunnerDestroyed(const Runner & runner):
myRunner(runner)
{}

template <class Runner>
const Runner & PipeRunnerDestroyed<Runner>::getRunner() const
{
  return myRunner;
}

}
}

#endif /* PIPE_RUNNER_EVENTS_DETAIL_H */
