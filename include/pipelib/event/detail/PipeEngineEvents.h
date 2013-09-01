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

template <class Runner>
PipeEngineDestroyed<Runner>::PipeEngineDestroyed(const Runner & runner):
myRunner(runner)
{}

template <class Runner>
const Runner & PipeEngineDestroyed<Runner>::getRunner() const
{
  return myRunner;
}

}
}

#endif /* PIPE_RUNNER_EVENTS_DETAIL_H */
