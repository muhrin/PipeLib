/*
 * Sinks.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SINKS_H
#define SINKS_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {

template< class Pipe>
  class FinishedSink
  {
  public:
    typedef typename UniquePtr< Pipe>::Type PipePtr;

    virtual
    ~FinishedSink()
    {
    }

    virtual void
    finished(PipePtr data) = 0;
  };

template< class Pipe>
  class DroppedSink
  {
  public:
    typedef typename UniquePtr< Pipe>::Type PipePtr;

    virtual
    ~DroppedSink()
    {
    }

    virtual void
    dropped(PipePtr data) = 0;
  };

}

#endif /* SINKS_H */
