/*
 * Barrier.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BARRIER_H
#define BARRIER_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/PipeBlock.h"


// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <typename PipelineData, typename SharedData, typename GlobalData>
class Barrier : public virtual PipeBlock<PipelineData, SharedData, GlobalData>
{
  typedef Block<PipelineData, SharedData, GlobalData> BlockType;
  typedef typename BlockType::RunnerSetupType RunnerSetupType;
public:
  Barrier() : BlockType("Barrier") {}

  /**
  /* Release all the data that this barrier is holding back.
  /*
  /**/
  virtual size_t release() = 0;

  virtual bool hasData() const = 0;

protected:
  virtual void runnerAttached(RunnerSetupType & setup)
  {
    setup.registerBarrier(*this);
  }
};

}

#endif /* BARRIER_H */
