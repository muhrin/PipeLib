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
#include "Pipeline.h"
#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {
	template <class DataType, class GlobalDataType>
	class IPipeline;

	template <class DataType, class GlobalDataType>
	class PipeBlock;
}

namespace pipelib {

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
class Block
{
public:

	explicit Block(const ::std::string & name);
	virtual ~Block() {}

	const std::string & getName() const;

	/**
	/* Get the pipeline that this block is a member of.  Can return NULL if not set.
	/**/
	virtual const IPipeline<DataType, GlobalDataType> * getPipeline() const;

	/**
	/* Get the number of outputs that this block has.
	/**/
	virtual size_t getNumOutputs() const = 0;

	/**
	/* Set the output block for a particular channel.
	/**/
	virtual void setOutput(PipeBlock<DataType, GlobalDataType> & output, const ChannelTyp channel = CHANNEL_DEFAULT) = 0;

	/**
	/* Clear the output on a particular channel.
	/**/
	virtual void clearOutput(const ChannelTyp channel = CHANNEL_DEFAULT) = 0;

	/**
	/* Get the output on a particular channel.  Can be NULL if not set.
	/**/
	virtual PipeBlock<DataType, GlobalDataType> * getOutput(const ChannelTyp channel = CHANNEL_DEFAULT) const = 0;

	////////////////////////////////////////////
	// Pipeline messages to blocks
	////////////////////////////////////////////

	virtual void inserted(IPipeline<DataType, GlobalDataType> & pipeline);
	virtual void pipelineInitialising();
	virtual void pipelineInitialised();
	virtual void pipelineStarting();
	virtual void pipelineFinishing();
	virtual void removed();

protected:

	const ::std::string						myName;

	IPipeline<DataType, GlobalDataType> *	myPipeline;
};


// IMPLEMENTATION ///////////////////////////////////////////

template <class DataType, class GlobalDataType>
Block<DataType, GlobalDataType>::Block(const ::std::string & name):
myName(name),
myPipeline(NULL)
{}

template <class DataType, class GlobalDataType>
const ::std::string & Block<DataType, GlobalDataType>::getName() const
{
	return myName;
}

template <class DataType, class GlobalDataType>
const IPipeline<DataType, GlobalDataType> * Block<DataType, GlobalDataType>::getPipeline() const
{
	return myPipeline;
}

template <class DataType, class GlobalDataType>
void Block<DataType, GlobalDataType>::inserted(IPipeline<DataType, GlobalDataType> & pipeline)
{
	myPipeline = &pipeline;
}

template <class DataType, class GlobalDataType>
void Block<DataType, GlobalDataType>::removed()
{
	myPipeline = NULL;
}

template <class DataType, class GlobalDataType>
void Block<DataType, GlobalDataType>::pipelineInitialising()
{}

template <class DataType, class GlobalDataType>
void Block<DataType, GlobalDataType>::pipelineInitialised()
{}

template <class DataType, class GlobalDataType>
void Block<DataType, GlobalDataType>::pipelineStarting()
{}

template <class DataType, class GlobalDataType>
void Block<DataType, GlobalDataType>::pipelineFinishing()
{}

}

#endif /* BLOCK_H */
