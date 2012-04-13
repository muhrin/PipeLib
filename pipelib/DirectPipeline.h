/*
 * DirectPipeline.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef DIRECT_PIPELINE_H
#define DIRECT_PIPELINE_H

// INCLUDES /////////////////////////////////////////////
#include "Pipeline.h"
#include "AbstractPipeline.h"

#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////


namespace pipelib {

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
class DirectPipeline : public AbstractPipeline<DataType, GlobalDataType>
{
public:

	// From IPipeline /////////////////////////

	virtual void connect(Block<DataType, GlobalDataType> & output, PipeBlock<DataType, GlobalDataType> & input, const int outChannel = CHANNEL_DEFAULT);

	virtual bool initialise();

	virtual bool disconnect(Block<DataType, GlobalDataType> & output, const int outChannel = CHANNEL_DEFAULT);

	virtual bool hasBlock(Block<DataType, GlobalDataType> & block) const;

	// End IPipeline ///////////////////////////

private:

	typedef AbstractPipeline<DataType, GlobalDataType> MyAbsPipeTyp;

};

// IMPLEMENTATION ///////////////////////////
template <class DataType, class GlobalDataType>
void DirectPipeline<DataType, GlobalDataType>::connect(
	Block<DataType, GlobalDataType> & outputter,
	PipeBlock<DataType, GlobalDataType> & inputtee,
	const int outChannel)
{
	PASSERT(outChannel != CHANNEL_ANY);

	if(outChannel == CHANNEL_ALL)
	{
		// Need to connect on all output channels
		for(size_t i = 0; i < outputter.getNumOutputs(); ++i)
		{
			connect(outputter, inputtee, i);
		}
		return;
	}

	MyAbsPipeTyp::myBlocks.push_back(&outputter);
	MyAbsPipeTyp::myBlocks.push_back(&inputtee);
	outputter.setOutput(inputtee, outChannel);
	outputter.inserted(*this);
	inputtee.inserted(*this);
}

template <class DataType, class GlobalDataType>
bool DirectPipeline<DataType, GlobalDataType>::initialise()
{
	using std::vector;

	for(typename MyAbsPipeTyp::BlocksMap::iterator it = MyAbsPipeTyp::myBlocks.begin(),
		end = MyAbsPipeTyp::myBlocks.end(); it != end; ++it)
	{
		(*it)->pipelineInitialised();
	}

	return true;
}

template <class DataType, class GlobalDataType>
bool DirectPipeline<DataType, GlobalDataType>::disconnect(
	Block<DataType, GlobalDataType> & outputter,
	const int outChannel)
{
	PASSERT(outChannel != CHANNEL_ANY);

	if(outChannel == CHANNEL_ALL)
	{
		// Need to connect on all output channels
		for(size_t i = 0; i < outputter.getNumOutputs(); ++i)
		{
			disconnect(outputter, i);
		}
		return true;
	}

	outputter.clearOutput(outChannel);

	return true;
}

template <class DataType, class GlobalDataType>
bool DirectPipeline<DataType, GlobalDataType>::hasBlock(Block<DataType, GlobalDataType> &block) const
{
	// TODO: Write this
	return false;
}

}

#endif /* DIRECT_PIPELINE_H */
