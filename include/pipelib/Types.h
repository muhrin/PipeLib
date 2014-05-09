/*
 * Types.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPELIB_TYPES_H
#define PIPELIB_TYPES_H

// INCLUDES /////////////////////////////////////////////////
#include <memory>

#include <boost/noncopyable.hpp>

#include "pipelib/PipelibConfig.h"

namespace pipelib {

// FORWARD DECLARES ///////////////////////////////
template <typename Pipe, typename Shared, typename Global>
class Block;

template <typename Pipe, typename Shared, typename Global>
class PipeBlock;

template <typename Pipe, typename Shared, typename Global>
class BoostThreadEngine;

template <typename Pipe, typename Shared, typename Global>
class SerialEngine;

template <typename Pipe, typename Shared, typename Global>
class StartBlock;

// Channel 
typedef int Channel;
static const int CHANNEL_ALL  = -1;
static const int CHANNEL_DEFAULT = 0;

template <typename Pipe>
struct NoSharedGlobal
{
  typedef PipeBlock<Pipe, const void *, const void *> PipeBlockType;
  typedef StartBlock<Pipe, const void *, const void *> StartBlockType;
  typedef SerialEngine<Pipe, const void *, const void *> SerialEngineType;
#ifdef PIPELIB_USE_BOOST_THREAD
  typedef BoostThreadEngine<Pipe, const void *, const void *> BoostThreadEngineType;
#endif
};

// If C++11 is available then use std::unique_ptr, otherwise
// std::auto_ptr is used.  This means that the user should tailor
// their code using the define if they want to use any specific
// behaviour
template <typename T>
struct UniquePtr : ::boost::noncopyable
{
#ifdef PIPELIB_USE_CPP11
  typedef ::std::unique_ptr<T> Type;
#else
  typedef ::std::auto_ptr<T> Type;
#endif
private:
  UniquePtr() {}
};

// Sometimes it's not possible to use the above struct-typedef method,
// for example in cases where ADL should be used.  In which case this
// macro can be used for the pointer type instead.  It's less safe -
// but it works.
#ifdef PIPELIB_USE_CPP11
#  define PIPELIB_UNIQUE_PTR(T) ::std::unique_ptr<T>
#else
#  define PIPELIB_UNIQUE_PTR(T) ::std::auto_ptr<T>
#endif

template <typename T>
inline PIPELIB_UNIQUE_PTR(T) makeUniquePtr(T * ptr)
{
  return typename UniquePtr<T>::Type(ptr);
}

}


#endif /* PIPELIB_TYPES_H */
