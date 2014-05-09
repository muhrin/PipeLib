/*
 * PipeBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_BLOCK_H
#define PIPE_BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Block.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template< typename Pipe, typename Shared, typename Global>
  class PipeBlock : public virtual Block< Pipe, Shared, Global>
  {
  protected:
    typedef Block< Pipe, Shared, Global> Base;

  public:
    PipeBlock() :
        Base("Pipe block")
    {
    }
    virtual
    ~PipeBlock()
    {
    }

    virtual void
    in(Pipe * data) = 0;

    virtual PipeBlock *
    asPipeBlock()
    {
      return this;
    }
    virtual const PipeBlock *
    asPipeBlock() const
    {
      return this;
    }
  };

}

#endif /* PIPE_BLOCK_H */
