/*
 * AbstractSimpleBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef ABSTRACT_SIMPLE_BLOCK_H
#define ABSTRACT_SIMPLE_BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"
#include "pipelib/PipeBlock.h"

#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class AbstractSimpleBlock : public virtual PipeBlock<PipelineData, SharedData, GlobalData>
{
public:
	AbstractSimpleBlock();
	
	// From Block /////////////////////////////

	virtual size_t getNumOutputs() const;
	virtual PipeBlock<PipelineData, SharedData, GlobalData> * getOutput(const ChannelTyp channel = CHANNEL_DEFAULT) const;
	virtual void setOutput(PipeBlock<PipelineData, SharedData, GlobalData> & output, const ChannelTyp channel = CHANNEL_DEFAULT);
	virtual void clearOutput(const ChannelTyp channel = CHANNEL_DEFAULT);
	
	// End From Block ///////////////////////////

protected:

	PipeBlock<PipelineData, SharedData, GlobalData> *	myOutput;
};


// IMPLEMENTATION ///////////////////////////////////
template <typename PipelineData, typename SharedData, typename GlobalData>
AbstractSimpleBlock<PipelineData, SharedData, GlobalData>::AbstractSimpleBlock():
Block<PipelineData, SharedData, GlobalData>("Abstract simple block"),
myOutput(NULL)
{}

template <typename PipelineData, typename SharedData, typename GlobalData>
size_t AbstractSimpleBlock<PipelineData, SharedData, GlobalData>::getNumOutputs() const
{
	return 1;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void AbstractSimpleBlock<PipelineData, SharedData, GlobalData>::setOutput(PipeBlock<PipelineData, SharedData, GlobalData> & output, const ChannelTyp channel)
{
	PASSERT(channel == 0);

	myOutput = &output;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void AbstractSimpleBlock<PipelineData, SharedData, GlobalData>::clearOutput(const ChannelTyp channel)
{
	if(!(channel == CHANNEL_DEFAULT || channel == CHANNEL_ALL))
	{
		throw "Trying to clear simple block output on non-deafult channel.";
	}

	myOutput = NULL;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
PipeBlock<PipelineData, SharedData, GlobalData> * AbstractSimpleBlock<PipelineData, SharedData, GlobalData>::getOutput(const ChannelTyp channel) const
{
	PASSERT(channel == 0);

	return myOutput;
}

}

#endif /* ABSTRACT_SIMPLE_BLOCK_H */
