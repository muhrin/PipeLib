/*
 * PipelineState.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPELINE_STATE_H
#define PIPELINE_STATE_H

// INCLUDES /////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib
{

struct PipelineState
{
  enum Value
  {
    // The pipeline has not been initialised yet, changes can be made
    UNINITIALISED = 0,
    // The pipeline has been initialised, changes cannot be made
    INITIALISED,
    // The pipeline is running
    RUNNING,
    // The pipeline has stopped
    STOPPED,
    // The pipeline has run through and has finished ready to be started again
    FINISHED
  };
};



}

#endif /* PIPELINE_STATE_H */
