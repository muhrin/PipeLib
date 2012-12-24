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
#include "pipelib/PipeRunner.h"

#include <string>

#include <boost/scoped_array.hpp>

namespace pipelib {

// FORWARD DECLARATIONS ////////////////////////////////////
template <class BlockType>
class PreorderIncrementer;

template <class BlockType, class Incrementer>
class BlockIterator;

template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeBlock;

template <typename PipelineData, typename SharedData, typename GlobalData>
class Block
{
public:

  typedef PipelineData PipelineDataType;
  typedef SharedData SharedDataType;
  typedef GlobalData GlobalDataType;

  typedef PipeBlock<PipelineData, SharedData, GlobalData> PipeBlockType;
  typedef BlockConnector<PipelineData, SharedData, GlobalData> ConnectorType;

private:

  typedef ::boost::scoped_array<PipeBlockType *> Outputs;

protected:

  typedef typename PipeRunnerTypes<Block>::Setup RunnerSetupType;
  typedef typename PipeRunnerTypes<Block>::Access RunnerAccessType;

public:
  typedef BlockIterator<Block, PreorderIncrementer<Block> > PreorderIterator;
  typedef BlockIterator<Block, PreorderIncrementer<const Block> > ConstPreorderIterator;

  typedef typename Outputs::element_type * OutputIterator;
  typedef const typename Outputs::element_type * ConstOutputIterator;

	explicit Block(const ::std::string & name, const size_t numOutputs = 1);
	virtual ~Block() {}

	const std::string & getName() const;

	/**
	/* Get the number of outputs that this block has.
	/**/
	size_t getNumOutputs() const;

	/**
	/* Clear the output on a particular channel.
	/**/
	bool clearOutput(const Channel channel = CHANNEL_DEFAULT);

	/**
	/* Get the output on a particular channel.  Can be NULL if not set.
	/**/
	PipeBlockType * getOutput(const Channel channel = CHANNEL_DEFAULT) const;

  OutputIterator beginOutputs();
  OutputIterator endOutputs();

  ConstOutputIterator beginOutputs() const;
  ConstOutputIterator endOutputs() const;

  Block & operator |= (PipeBlockType & toConnect);
  ConnectorType operator[] (const Channel channel);

  PreorderIterator beginPreorder();
  PreorderIterator endPreorder();

  ConstPreorderIterator beginPreorder() const;
  ConstPreorderIterator endPreorder() const;

  ////////////////////////////////////////////
  // Pipeline runner messages to blocks
  ////////////////////////////////////////////
  void notifyAttached(RunnerSetupType & setup);
  void notifyInitialising(RunnerAccessType & access);
  void notifyInitialised();
  void notifyStarting();
  void notifyFinishing();
  void notifyFinished(RunnerAccessType & access);
  void notifyDetached();

  /**
  /* Set the output block for a particular channel.
  /**/
  void setOutput(PipeBlockType & output, const Channel channel = CHANNEL_DEFAULT);

protected:

  /**
  /* Get the PipeRunner driving this block.  Can return NULL if not running.
  /**/
  RunnerAccessType * getRunner();
  const RunnerAccessType * getRunner() const;

  void out(PipelineData & data, const Channel channel = CHANNEL_DEFAULT) const;

  ////////////////////////////////////////////
  // Pipeline runner messages to blocks
  ////////////////////////////////////////////
  virtual void runnerAttached(RunnerSetupType & /*setup*/) {}
  virtual void pipelineInitialising() {}
  virtual void pipelineInitialised() {}
  virtual void pipelineStarting() {}
  virtual void pipelineFinishing() {}
  virtual void pipelineFinished() {}
  virtual void runnerDetached() {}

private:

  const ::std::string myName;
  RunnerAccessType * myRunner;
  const size_t myNumOutputs;
  Outputs myOutputs;
};

}

#include "pipelib/detail/Block.h"

#endif /* BLOCK_H */
