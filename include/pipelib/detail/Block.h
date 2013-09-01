/*
 * Block.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BLOCK_DETAIL_H
#define BLOCK_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <list>
#include <set>

#include <boost/foreach.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include "pipelib/Pipeline.h"
#include "pipelib/BlockConnector.h"
#include "pipelib/BlockIterator.h"
#include "pipelib/PipeEngine.h"

namespace pipelib {

template< typename Pipe, typename Shared, typename Global>
  Block< Pipe, Shared, Global>::Block(const ::std::string & name) :
      myName(name), myOutputs(1), connect(this)
  {
  }

template< typename Pipe, typename Shared, typename Global>
  Block< Pipe, Shared, Global>::Block(const ::std::string & name, const size_t numOutputs) :
      myName(name), myOutputs(numOutputs), connect(*this)
  {
  }

template< typename Pipe, typename Shared, typename Global>
  const ::std::string &
  Block< Pipe, Shared, Global>::getName() const
  {
    return myName;
  }

template< typename Pipe, typename Shared, typename Global>
  size_t
  Block< Pipe, Shared, Global>::getNumOutputs() const
  {
    return myOutputs.size();
  }

template< typename Pipe, typename Shared, typename Global>
  bool
  Block< Pipe, Shared, Global>::clearOutput(const Channel channel)
  {
    PIPELIB_ASSERT(channel < static_cast<Channel>(getNumOutputs()));

    const bool wasSet = (myOutputs[channel] != NULL);
    myOutputs[channel] = NULL;
    return wasSet;
  }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::HandleType
  Block< Pipe, Shared, Global>::getOutput(const Channel channel) const
  {
    PIPELIB_ASSERT(channel < (int)getNumOutputs());

    return myOutputs[channel];
  }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::OutputIterator
  Block< Pipe, Shared, Global>::beginOutputs()
  {
    return myOutputs.begin();
  }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::OutputIterator
  Block< Pipe, Shared, Global>::endOutputs()
  {
    return myOutputs.end();
  }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::ConstOutputIterator
  Block< Pipe, Shared, Global>::beginOutputs() const
  {
    return myOutputs.begin();
  }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::ConstOutputIterator
  Block< Pipe, Shared, Global>::endOutputs() const
  {
    return myOutputs.end();
  }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::PreorderIterator
  Block< Pipe, Shared, Global>::beginPreorder()
  {
    return PreorderIterator(getHandle());
  }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::PreorderIterator
  Block< Pipe, Shared, Global>::endPreorder()
  {
    return PreorderIterator();
  }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::ConstPreorderIterator
  Block< Pipe, Shared, Global>::beginPreorder() const
  {
    ConstPreorderIterator (
    getHandle());}

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::ConstPreorderIterator
  Block< Pipe, Shared, Global>::endPreorder() const
  {
    ConstPreorderIterator();
  }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::EngineAccessType *
  Block< Pipe, Shared, Global>::getEngine()
  {
    return myEngine;
  }

template< typename Pipe, typename Shared, typename Global>
  const typename Block< Pipe, Shared, Global>::EngineAccessType *
  Block< Pipe, Shared, Global>::getEngine() const
  {
    return myEngine;
  }

template< typename Pipe, typename Shared, typename Global>
  void
  Block< Pipe, Shared, Global>::out(Pipe * const data) const
  {
    // Tell the engine to pass on the data
    myEngine->out(data, *this, CHANNEL_DEFAULT);
  }

template< typename Pipe, typename Shared, typename Global>
  void
  Block< Pipe, Shared, Global>::out(Pipe * const data, const Channel channel) const
  {
    // Tell the engine to pass on the data
    myEngine->out(data, *this, channel);
  }

template< typename Pipe, typename Shared, typename Global>
  void
  Block< Pipe, Shared, Global>::drop(Pipe * const data) const
  {
    // Tell the engine to pass on the data
    myEngine->dropData(data);
  }

template< typename Pipe, typename Shared, typename Global>
  bool
  Block< Pipe, Shared, Global>::isPipeBlock() const
  {
    return asPipeBlock() != NULL;
  }

template< typename Pipe, typename Shared, typename Global>
  bool
  Block< Pipe, Shared, Global>::isStartBlock() const
  {
    return asStartBlock() != NULL;
  }

template< typename Pipe, typename Shared, typename Global>
  template< typename Visitor>
    void
    Block< Pipe, Shared, Global>::visitBlocks(Visitor & visitor)
    {
      visitBlocks(visitor, 0);
    }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::HandleType
  Block< Pipe, Shared, Global>::doConnect(HandleType & to)
  {
    PIPELIB_ASSERT_MSG(!getEngine(), "Can't connect pipe blocks while engine is attached.");
    PIPELIB_ASSERT_MSG(to->asPipeBlock(), "Can only blocks to pipe blocks.");

    myOutputs[CHANNEL_DEFAULT] = to;
    return to;
  }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::HandleType
  Block< Pipe, Shared, Global>::doConnect(HandleType & to, const Channel channel)
  {
    PIPELIB_ASSERT_MSG(channel < (int)getNumOutputs(), "Channel out of range");
    PIPELIB_ASSERT_MSG(!getEngine(), "Can't connect pipe blocks while engine is attached.");
    PIPELIB_ASSERT_MSG(to->asPipeBlock(), "Can only blocks to pipe blocks.");

    myOutputs[channel] = to;
    return to;
  }

template< typename Pipe, typename Shared, typename Global>
  typename Block< Pipe, Shared, Global>::HandleType
  Block< Pipe, Shared, Global>::getHandle()
  {
    return this->shared_from_this();
  }

/////////////////////////////////////
// PipeEngine messages //////////////

template< typename Pipe, typename Shared, typename Global>
  void
  Block< Pipe, Shared, Global>::notifyAttached(EngineSetupType * const setup)
  {
    // Pass on the message
    engineAttached(setup);
  }

template< typename Pipe, typename Shared, typename Global>
  void
  Block< Pipe, Shared, Global>::notifyInitialising(EngineAccessType * const access)
  {
    myEngine = access;
    pipelineInitialising();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  Block< Pipe, Shared, Global>::notifyInitialised()
  {
    // Pass on the message
    pipelineInitialised();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  Block< Pipe, Shared, Global>::notifyStarting()
  {
    // Pass on the message
    pipelineStarting();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  Block< Pipe, Shared, Global>::notifyFinishing()
  {
    // Pass on the message
    pipelineFinishing();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  Block< Pipe, Shared, Global>::notifyFinished(EngineAccessType * access)
  {
    PIPELIB_ASSERT(myEngine == access);

    myEngine = NULL;
    // Pass on the message
    pipelineFinished();
  }

template< typename Pipe, typename Shared, typename Global>
  void
  Block< Pipe, Shared, Global>::notifyDetached()
  {
    myEngine = NULL;
    // Pass on the message
    engineDetached();
  }

template< typename Pipe, typename Shared, typename Global>
  template< typename Visitor>
    bool
    Block< Pipe, Shared, Global>::visitBlocks(Visitor & visitor, const int depth)
    {
      if(!visitor(getHandle(), depth))
        return false;

      BOOST_FOREACH(HandleType & output, myOutputs)
      {
        if(output.get() && !output->visitBlocks(visitor, depth + 1))
          return false;
      }
      return true;
    }

} // namespace pipelib

#endif /* BLOCK_H */
