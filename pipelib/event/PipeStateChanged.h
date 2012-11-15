/*
 * PipeStateChanged.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_STATE_CHANGED_H
#define PIPE_STATE_CHANGED_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/PipelineState.h"

namespace pipelib {
namespace event {


template <class Pipeline>
class PipeStateChanged
{
public:
  PipeStateChanged(
    const Pipeline & pipeline,
    const ::pipelib::PipelineState::Value  oldState,
    const ::pipelib::PipelineState::Value  newState);

  const Pipeline & getPipeline() const;
  ::pipelib::PipelineState::Value getOldState() const;
  ::pipelib::PipelineState::Value getNewState() const;
  

private:
  const Pipeline &                        myPipeline;
  const ::pipelib::PipelineState::Value   myOldState;
  const ::pipelib::PipelineState::Value   myNewState;

};

// IMPLEMENTATION ///////////////////////////////////////////////////

template <class Pipeline>
PipeStateChanged<Pipeline>::PipeStateChanged(
  const Pipeline & pipeline,
  const ::pipelib::PipelineState::Value oldState,
  const ::pipelib::PipelineState::Value newState):
myPipeline(pipeline),
myOldState(oldState),
myNewState(newState)
{}

template <class Pipeline>
const Pipeline &
PipeStateChanged<Pipeline>::getPipeline() const
{
  return myPipeline;
}

template <class Pipeline>
::pipelib::PipelineState::Value
PipeStateChanged<Pipeline>::getOldState() const
{
  return myOldState;
}

template <class Pipeline>
::pipelib::PipelineState::Value
PipeStateChanged<Pipeline>::getNewState() const
{
  return myNewState;
}

}
}

#endif /* PIPE_STATE_CHANGED_H */
