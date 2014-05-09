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

template <typename Pipe, typename Shared, typename Global>
class Barrier : public virtual PipeBlock<Pipe, Shared, Global>
{
  typedef Block<Pipe, Shared, Global> BlockType;
  typedef typename BlockType::EngineSetupType EngineSetupType;
public:
  Barrier() : BlockType("Barrier") {}
  virtual ~Barrier() {}

  // Release all the data that this barrier is holding back.
  virtual size_t release() = 0;

  virtual bool hasData() const = 0;

protected:
  virtual void
  engineAttached(EngineSetupType * const setup)
  {
    setup->registerBarrier(this);
  }
};

}

#endif /* BARRIER_H */
