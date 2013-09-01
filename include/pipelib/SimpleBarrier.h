/*
 * SimpleBarrier.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SIMPLE_BARRIER_H
#define SIMPLE_BARRIER_H

// INCLUDES /////////////////////////////////////////////
#include <vector>

#include <pipelib/Barrier.h>

namespace pipelib {

template< typename Pipe, typename Shared, typename Global>
  class SimpleBarrier : public Barrier< Pipe, Shared, Global>
  {
  public:

    SimpleBarrier();
    virtual ~SimpleBarrier() {}

    virtual void
    in(Pipe * const data);

    virtual size_t
    release();

    virtual bool
    hasData() const;

  protected:
    typedef ::std::vector< Pipe *> Buffer;

    Buffer myBuffer;
  };

}

#include "pipelib/detail/SimpleBarrier.h"

#endif /* SIMPLE_BARRIER_H */
