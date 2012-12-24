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

namespace pipelib {

// FORWARD DECLARES ///////////////////////////////
template <typename PipelineData, typename SharedData, typename GlobalData>
class Block;

template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeBlock;

template <typename PipelineData, typename SharedData, typename GlobalData>
class SingleThreadedEngine;

template <typename PipelineData, typename SharedData, typename GlobalData>
class StartBlock;

// Channel 
typedef int Channel;
static const int CHANNEL_NONE = -3;
static const int CHANNEL_ANY  = -2;
static const int CHANNEL_ALL  = -1;
static const int CHANNEL_DEFAULT = 0;

// Data handle
typedef size_t PipelineDataHandle;

template <typename PipelineData>
struct NoSharedGlobal
{
  typedef PipeBlock<PipelineData, const void *, const void *> PipeBlockType;
  typedef StartBlock<PipelineData, const void *, const void *> StartBlockType;
  typedef SingleThreadedEngine<PipelineData, const void *, const void *> SingleThreadedEngineType;
};

// If C++11 is available then use std::unique_ptr, otherwise
// std::auto_ptr is used.  This means that the user should taylor
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
	
}


#endif /* PIPELIB_TYPES_H */
