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
#include "pipelib/Pipeline.h"
#include "pipelib/AbstractPipeline.h"

#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////


namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class DirectPipeline : public AbstractPipeline<PipelineData, SharedData, GlobalData>
{
public:

	// From IPipeline /////////////////////////

	virtual void connect(Block<PipelineData, SharedData, GlobalData> & output, PipeBlock<PipelineData, SharedData, GlobalData> & input, const int outChannel = CHANNEL_DEFAULT);

	virtual bool initialise();

	virtual bool disconnect(Block<PipelineData, SharedData, GlobalData> & output, const int outChannel = CHANNEL_DEFAULT);

	virtual bool hasBlock(Block<PipelineData, SharedData, GlobalData> & block) const;

	// End IPipeline ///////////////////////////

private:

	typedef AbstractPipeline<PipelineData, SharedData, GlobalData> MyAbsPipeTyp;

};

// IMPLEMENTATION ///////////////////////////
template <typename PipelineData, typename SharedData, typename GlobalData>
void DirectPipeline<PipelineData, SharedData, GlobalData>::connect(
	Block<PipelineData, SharedData, GlobalData> & outputter,
	PipeBlock<PipelineData, SharedData, GlobalData> & inputtee,
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

template <typename PipelineData, typename SharedData, typename GlobalData>
bool DirectPipeline<PipelineData, SharedData, GlobalData>::initialise()
{
	using std::vector;

	for(typename MyAbsPipeTyp::BlocksMap::iterator it = MyAbsPipeTyp::myBlocks.begin(),
		end = MyAbsPipeTyp::myBlocks.end(); it != end; ++it)
	{
		(*it)->pipelineInitialised();
	}

	return true;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
bool DirectPipeline<PipelineData, SharedData, GlobalData>::disconnect(
	Block<PipelineData, SharedData, GlobalData> & outputter,
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

template <typename PipelineData, typename SharedData, typename GlobalData>
bool DirectPipeline<PipelineData, SharedData, GlobalData>::hasBlock(Block<PipelineData, SharedData, GlobalData> &block) const
{
	// TODO: Write this
	return false;
}

}

#endif /* DIRECT_PIPELINE_H */
