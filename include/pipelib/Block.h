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
#include <string>


// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {

template <typename PipelineData, typename SharedData, typename GlobalData>
class IPipeline;

template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeBlock;
}

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class Block
{
public:

	explicit Block(const ::std::string & name);
	virtual ~Block() {}

	const std::string & getName() const;

	/**
	/* Get the pipeline that this block is a member of.  Can return NULL if not set.
	/**/
	virtual const IPipeline<PipelineData, SharedData, GlobalData> * getPipeline() const;

	/**
	/* Get the number of outputs that this block has.
	/**/
	virtual size_t getNumOutputs() const = 0;

	/**
	/* Set the output block for a particular channel.
	/**/
	virtual void setOutput(PipeBlock<PipelineData, SharedData, GlobalData> & output, const ChannelTyp channel = CHANNEL_DEFAULT) = 0;

	/**
	/* Clear the output on a particular channel.
	/**/
	virtual void clearOutput(const ChannelTyp channel = CHANNEL_DEFAULT) = 0;

	/**
	/* Get the output on a particular channel.  Can be NULL if not set.
	/**/
	virtual PipeBlock<PipelineData, SharedData, GlobalData> * getOutput(const ChannelTyp channel = CHANNEL_DEFAULT) const = 0;

	////////////////////////////////////////////
	// Pipeline messages to blocks
	////////////////////////////////////////////

	virtual void inserted(IPipeline<PipelineData, SharedData, GlobalData> & pipeline);
  virtual void pipelineInitialising() {}
  virtual void pipelineInitialised() {}
  virtual void pipelineStarting() {}
  virtual void pipelineFinishing() {}
	virtual void removed();

protected:

	const ::std::string						myName;

	IPipeline<PipelineData, SharedData, GlobalData> *	myPipeline;
};


// IMPLEMENTATION ///////////////////////////////////////////

template <typename PipelineData, typename SharedData, typename GlobalData>
Block<PipelineData, SharedData, GlobalData>::Block(const ::std::string & name):
myName(name),
myPipeline(NULL)
{}

template <typename PipelineData, typename SharedData, typename GlobalData>
const ::std::string & Block<PipelineData, SharedData, GlobalData>::getName() const
{
	return myName;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const IPipeline<PipelineData, SharedData, GlobalData> * Block<PipelineData, SharedData, GlobalData>::getPipeline() const
{
	return myPipeline;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void Block<PipelineData, SharedData, GlobalData>::inserted(IPipeline<PipelineData, SharedData, GlobalData> & pipeline)
{
	myPipeline = &pipeline;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void Block<PipelineData, SharedData, GlobalData>::removed()
{
	myPipeline = NULL;
}

}

#endif /* BLOCK_H */
