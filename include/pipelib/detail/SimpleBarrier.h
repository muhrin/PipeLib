/*
 * SimpleBarrier.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SIMPLE_BARRIER_DETAIL_H
#define SIMPLE_BARRIER_DETAIL_H

// INCLUDES /////////////////////////////////////////////

#include <boost/foreach.hpp>

namespace pipelib {

template< typename Pipe, typename Shared, typename Global>
  SimpleBarrier< Pipe, Shared, Global>::SimpleBarrier() :
      Block< Pipe, Shared, Global>("Simple barrier")
  {
  }

template< typename Pipe, typename Shared, typename Global>
  void
  SimpleBarrier< Pipe, Shared, Global>::in(Pipe * const data)
  {
    myBuffer.push_back(data);
    // Just return, don't pass it on:  This will be done upon release()
  }

template< typename Pipe, typename Shared, typename Global>
  size_t
  SimpleBarrier< Pipe, Shared, Global>::release()
  {
    const size_t bufferSize = myBuffer.size();
    BOOST_FOREACH(Pipe * const data, myBuffer)
    {
      this->out(data);
    }

    // Done our job, so clear the buffer
    myBuffer.clear();

    return bufferSize;
  }

template< typename Pipe, typename Shared, typename Global>
  bool
  SimpleBarrier< Pipe, Shared, Global>::hasData() const
  {
    return !myBuffer.empty();
  }

}

#endif /* SIMPLE_BARRIER_DETAIL_H */
