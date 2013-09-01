/*
 * AbsPipeEngine.h
 *
 *
 *  Created on: Aug 31, 2013
 *      Author: Martin Uhrin
 */

#ifndef ABS_PIPE_ENGINE_H
#define ABS_PIPE_ENGINE_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"
#include "pipelib/PipeEngine.h"

namespace pipelib {


template <typename Pipe, typename Shared, typename Global>
class AbsPipeEngine : public PipeEngine<Pipe, Shared, Global>
{

  typedef PipeEngine<Pipe, Shared, Global> Base;

protected:

  template <typename BlockIterator>
  void notifyAttached(BlockIterator & first, BlockIterator & last, SetupType * const setup);
  template <typename BlockIterator>
  void notifyInitialising(BlockIterator & first, BlockIterator & last, AccessType * const access);
  template <typename BlockIterator>
  void notifyInitialised(BlockIterator & first, BlockIterator & last);
  template <typename BlockIterator>
  void notifyStarting(BlockIterator & first, BlockIterator & last);
  template <typename BlockIterator>
  void notifyFinishing(BlockIterator & first, BlockIterator & last);
  template <typename BlockIterator>
  void notifyFinished(BlockIterator & first, BlockIterator & last, AccessType * const access);
  template <typename BlockIterator>
  void notifyDetached(BlockIterator & first, BlockIterator & last);
};


}

#include "pipelib/detail/AbsPipeEngine.h"

#endif /* ABS_PIPE_ENGINE_H */
