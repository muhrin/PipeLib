/*
 * StartBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef START_BLOCK_H
#define START_BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Block.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template< typename Pipe, typename Shared, typename Global>
  class StartBlock : public virtual Block< Pipe, Shared, Global>
  {
  protected:
    typedef Block< Pipe, Shared, Global> BlockType;

  public:
    StartBlock() :
        BlockType("Start block")
    {
    }
    virtual
    ~StartBlock()
    {
    }

    virtual void
    start() = 0;

    virtual StartBlock *
    asStartBlock()
    {
      return this;
    }
    virtual const StartBlock *
    asStartBlock() const
    {
      return this;
    }
  };

}

#endif /* START_BLOCK_H */
