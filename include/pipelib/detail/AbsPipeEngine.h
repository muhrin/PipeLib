/*
 * AbsPipeEngine.h
 *
 *
 *  Created on: Aug 31, 2013
 *      Author: Martin Uhrin
 */

#ifndef ABS_PIPE_ENGINE_DETAIL_H
#define ABS_PIPE_ENGINE_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <algorithm>

namespace pipelib {


template <typename Pipe, typename Shared, typename Global>
template <typename BlockIterator>
void AbsPipeEngine<Pipe, Shared, Global>::notifyAttached(BlockIterator & first, BlockIterator & last, SetupType * const setup)
{
  
}

template <typename Pipe, typename Shared, typename Global>
template <typename BlockIterator>
void AbsPipeEngine<Pipe, Shared, Global>notifyInitialising(BlockIterator & first, BlockIterator & last, AccessType * const access)
{
  
}

template <typename Pipe, typename Shared, typename Global>
template <typename BlockIterator>
void AbsPipeEngine<Pipe, Shared, Global>notifyInitialised(BlockIterator & first, BlockIterator & last)
{
  ::std::for_each(first, last, Base::notifyInitialised);
}

template <typename Pipe, typename Shared, typename Global>
template <typename BlockIterator>
void AbsPipeEngine<Pipe, Shared, Global>notifyStarting(BlockIterator & first, BlockIterator & last)
{
  ::std::for_each(first, last, Base::notifyInitialised);
}

template <typename Pipe, typename Shared, typename Global>
template <typename BlockIterator>
void AbsPipeEngine<Pipe, Shared, Global>notifyFinishing(BlockIterator & first, BlockIterator & last)
{
  ::std::for_each(first, last, Base::notifyInitialised);
}

template <typename Pipe, typename Shared, typename Global>
template <typename BlockIterator>
void AbsPipeEngine<Pipe, Shared, Global>notifyFinished(BlockIterator & first, BlockIterator & last, AccessType * const access)
{
  
}

template <typename Pipe, typename Shared, typename Global>
template <typename BlockIterator>
void AbsPipeEngine<Pipe, Shared, Global>notifyDetached(BlockIterator & first, BlockIterator & last)
{
  
}

}


#endif /* ABS_PIPE_ENGINE_DETAIL_H */
