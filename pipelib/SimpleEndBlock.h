/*
 * SimpleEndBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SIMPLE_END_BLOCK_H
#define SIMPLE_END_BLOCK_H

// INCLUDES /////////////////////////////////////////////

#include "pipelib/IFinishNotifiable.h"
#include "pipelib/PipeBlock.h"


#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class SimpleEndBlock :	public PipeBlock<PipelineData, SharedData, GlobalData>
{
public:

	SimpleEndBlock(IFinishNotifiable<PipelineData> & notifiable);

	// From Block ////////////////////////

	virtual size_t getNumOutputs() const;
	virtual void setOutput(PipeBlock<PipelineData, SharedData, GlobalData> & output, const ChannelTyp channel = CHANNEL_DEFAULT);
	virtual PipeBlock<PipelineData, SharedData, GlobalData> * getOutput(const ChannelTyp channel = CHANNEL_DEFAULT) const;
	virtual void clearOutput(const ChannelTyp channel = CHANNEL_DEFAULT);

	// End from Block ////////////////////////

	// From PipeBlock ////////////////////////

	virtual void in(PipelineData & data);

	// End from PipeBlock ////////////////////

protected:
	IFinishNotifiable<PipelineData> &	myNotifiable;
};

// IMPLEMENTATION //////////////////////////////////////////

template <typename PipelineData, typename SharedData, typename GlobalData>
SimpleEndBlock<PipelineData, SharedData, GlobalData>::SimpleEndBlock(IFinishNotifiable<PipelineData> & notifiable):
Block<PipelineData, SharedData, GlobalData>("Simple end block"),
myNotifiable(notifiable)
{}


template <typename PipelineData, typename SharedData, typename GlobalData>
size_t SimpleEndBlock<PipelineData, SharedData, GlobalData>::getNumOutputs() const
{
	return 0;	// End block - no outputs
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SimpleEndBlock<PipelineData, SharedData, GlobalData>::setOutput(PipeBlock<PipelineData, SharedData, GlobalData> & output, const ChannelTyp channel)
{
	throw "Can't set output on end block";
}

template <typename PipelineData, typename SharedData, typename GlobalData>
PipeBlock<PipelineData, SharedData, GlobalData> *
SimpleEndBlock<PipelineData, SharedData, GlobalData>::getOutput(const ChannelTyp channel) const
{
	return NULL;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SimpleEndBlock<PipelineData, SharedData, GlobalData>::clearOutput(const ChannelTyp channel)
{
	throw "Cannot clear output on end block: it has no output";
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SimpleEndBlock<PipelineData, SharedData, GlobalData>::in(PipelineData & data)
{
	myNotifiable.dataFinished(data);
}

}

#endif /* SIMPLE_END_BLOCK_H */
