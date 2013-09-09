/*
 * Block.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BLOCK_H
#define BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"
#include "pipelib/BlockConnector.h"
#include "pipelib/BlockHandle.h"
#include "pipelib/PipeEngine.h"

#include <string>
#include <vector>

#include <boost/enable_shared_from_this.hpp>

namespace pipelib {

// FORWARD DECLARATIONS ////////////////////////////////////
template< class BlockType>
  class PreorderIncrementer;

template< class BlockType, class Incrementer>
  class BlockIterator;

template< typename Pipe, typename Shared, typename Global>
  class PipeBlock;

template< typename Pipe, typename Shared, typename Global>
  class StartEngine;

template< typename Pipe, typename Shared, typename Global>
  class StartBlock;

template< typename Pipe, typename Shared, typename Global>
  class Block : public ::boost::enable_shared_from_this<Block<Pipe, Shared, Global> >
  {
    typedef Connector<Pipe, Shared, Global> ConnectorType;
  public:

    typedef Pipe PipeType;
    typedef Shared SharedType;
    typedef Global GlobalType;
    typedef Block<Pipe, Shared, Global> Self;
    typedef typename BlockHandle<PipeType, SharedType, GlobalType>::Type HandleType;

    typedef PipeBlock< Pipe, Shared, Global> PipeBlockType;
    typedef StartBlock< Pipe, Shared, Global> StartBlockType;

  private:
    typedef ::std::vector<HandleType> Outputs;

  protected:

    typedef typename PipeEngineTypes< Block>::Setup EngineSetupType;
    typedef typename PipeEngineTypes< Block>::Access EngineAccessType;

  public:
    typedef BlockIterator< Block, PreorderIncrementer< Block> > PreorderIterator;
    typedef BlockIterator< Block, PreorderIncrementer< const Block> > ConstPreorderIterator;

    typedef typename Outputs::iterator OutputIterator;
    typedef const typename Outputs::const_iterator ConstOutputIterator;

    explicit
    Block(const ::std::string & name);
    Block(const ::std::string & name, const size_t numOutputs);
    virtual
    ~Block()
    {
    }

    const ::std::string &
    getName() const;

    ConnectorType connect;

    /**
     /* Get the number of outputs that this block has.
     /**/
    size_t
    getNumOutputs() const;

    /**
     /* Clear the output on a particular channel.
     /**/
    bool
    clearOutput(const Channel channel = CHANNEL_DEFAULT);

    /**
     /* Get the output on a particular channel.
     /**/
    HandleType
    getOutput(const Channel channel = CHANNEL_DEFAULT) const;

    OutputIterator
    beginOutputs();
    OutputIterator
    endOutputs();

    ConstOutputIterator
    beginOutputs() const;
    ConstOutputIterator
    endOutputs() const;

    PreorderIterator
    beginPreorder();
    PreorderIterator
    endPreorder();

    ConstPreorderIterator
    beginPreorder() const;
    ConstPreorderIterator
    endPreorder() const;

    bool
    isPipeBlock() const;
    bool
    isStartBlock() const;

    virtual PipeBlockType *
    asPipeBlock()
    {
      return NULL;
    }
    virtual const PipeBlockType *
    asPipeBlock() const
    {
      return NULL;
    }

    virtual StartBlockType *
    asStartBlock()
    {
      return NULL;
    }
    virtual const StartBlockType *
    asStartBlock() const
    {
      return NULL;
    }

    template< typename Visitor>
    void visitBlocks(Visitor & visitor);

  protected:

    /**
     /* Get the PipeEngine driving this block.  Can return NULL if not running.
     /**/
    EngineAccessType *
    getEngine();
    const EngineAccessType *
    getEngine() const;

    void
    out(Pipe * const data) const;
    void
    out(Pipe * const data, const Channel channel) const;

    void
    drop(Pipe * const data) const;

    ////////////////////////////////////////////
    // Pipeline engine messages to blocks
    ////////////////////////////////////////////
    virtual void
    engineAttached(EngineSetupType * /*setup*/)
    {
    }
    virtual void
    pipelineInitialising()
    {
    }
    virtual void
    pipelineInitialised()
    {
    }
    virtual void
    pipelineStarting()
    {
    }
    virtual void
    pipelineFinishing()
    {
    }
    virtual void
    pipelineFinished()
    {
    }
    virtual void
    engineDetached()
    {
    }

  private:

    HandleType
    doConnect(HandleType & to);
    /**
     /* Set the output block for a particular channel.
     /**/
    HandleType
    doConnect(HandleType & to, const Channel channel);

    HandleType getHandle();

    ////////////////////////////////////////////
    // Pipeline engine messages to blocks
    ////////////////////////////////////////////
    void
    notifyAttached(EngineSetupType * const setup);
    void
    notifyInitialising(EngineAccessType * const access);
    void
    notifyInitialised();
    void
    notifyStarting();
    void
    notifyFinishing();
    void
    notifyFinished(EngineAccessType * const access);
    void
    notifyDetached();

    template< typename Visitor>
    bool visitBlocks(Visitor & visitor, const int depth);

    const ::std::string myName;
    EngineAccessType * myEngine;
    Outputs myOutputs;

    friend class Connector<Pipe, Shared, Global>;
    friend class ChannelConnector<Pipe, Shared, Global>;
    friend class PipeEngine<Pipe, Shared, Global>;
  };

}

#include "pipelib/detail/Block.h"

#endif /* BLOCK_H */
