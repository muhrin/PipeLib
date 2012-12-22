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
namespace event {


template <class Runner>
class PipeRunnerStateChanged
{
public:
  PipeRunnerStateChanged(
    const Runner & pipeline,
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

template <class Pipeline>
PipeRunnerStateChanged<Pipeline>::PipeRunnerStateChanged(
  const Pipeline & pipeline,
  const ::pipelib::PipelineState::Value oldState,
  const ::pipelib::PipelineState::Value newState):
myPipeline(pipeline),
myOldState(oldState),
myNewState(newState)
{}

template <class Pipeline>
const Pipeline &
PipeRunnerStateChanged<Pipeline>::getRunner() const
{
  return myRunner;
}

template <class Pipeline>
::pipelib::PipelineState::Value
PipeRunnerStateChanged<Pipeline>::getOldState() const
{
  return myOldState;
}

template <class Pipeline>
::pipelib::PipelineState::Value
PipeRunnerStateChanged<Pipeline>::getNewState() const
{
  return myNewState;
}

}
}

#endif /* PIPE_STATE_CHANGED_H */
