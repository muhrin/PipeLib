/*
 * SimpleLink.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SIMPLE_LINK_H
#define SIMPLE_LINK_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"

#include "pipelib/AbstractLinkPipeline.h"
#include "pipelib/AbstractSimpleBlock.h"
#include "pipelib/ILink.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class SimpleLink : public AbstractSimpleBlock<PipelineData, SharedData, GlobalData>, public ILink<PipelineData, SharedData, GlobalData>
{
public:
	SimpleLink(AbstractLinkPipeline<PipelineData, SharedData, GlobalData> & pipeline);


	// From ILink ////////////////////////////////////////////////

	virtual void link(
		Block<PipelineData, SharedData, GlobalData> & input,
		PipeBlock<PipelineData, SharedData, GlobalData> & output,
		const ChannelTyp outChannel = CHANNEL_DEFAULT);

	virtual void unlink();

	virtual Block<PipelineData, SharedData, GlobalData> & getInput() const;

	ChannelTyp getLinkChannel() const;

	// End ILink //////////////////////////////////////////////////

	// From PipeBlock ////////////////////////////////////////////

	virtual void in(PipelineData & data);

	// End PipeBlock /////////////////////////////////////////////

protected:

	/** The pipeline that this link comminucates with. */
	AbstractLinkPipeline<PipelineData, SharedData, GlobalData> &	myLinkPipeline;

	/** The blocks that is attached to the input of this link. */
	Block<PipelineData, SharedData, GlobalData> *		myInput;

	/** The output channel of the outputter that this link takes as an input */
	ChannelTyp						myLinkChannel;
};

// IMPLEMENTATION /////////////////////////

template <typename PipelineData, typename SharedData, typename GlobalData>
SimpleLink<PipelineData, SharedData, GlobalData>::SimpleLink(AbstractLinkPipeline<PipelineData, SharedData, GlobalData> & pipeline):
Block<PipelineData, SharedData, GlobalData>("Simple link"),
myLinkPipeline(pipeline),
myInput(NULL),
myLinkChannel(CHANNEL_NONE)
{
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SimpleLink<PipelineData, SharedData, GlobalData>::link(
	Block<PipelineData, SharedData, GlobalData> & input,
	PipeBlock<PipelineData, SharedData, GlobalData> & output,
	const ChannelTyp outChannel)
{
	myInput = &input;
	myLinkChannel = outChannel;
	myInput->setOutput(*this, myLinkChannel);
	setOutput(output);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SimpleLink<PipelineData, SharedData, GlobalData>::unlink()
{
    // Have to use this-> as we're calling this in a template base class
	this->clearOutput();
	myInput->clearOutput(myLinkChannel);
	myLinkChannel = CHANNEL_NONE;
	myInput = NULL;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
Block<PipelineData, SharedData, GlobalData> & SimpleLink<PipelineData, SharedData, GlobalData>::getInput() const
{
	return *myInput;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SimpleLink<PipelineData, SharedData, GlobalData>::in(PipelineData & data)
{
	// Tell the pipeline that this link is passing data
	myLinkPipeline.linkCallback(*this, data);

	// Pass the data on to the next block
	this->myOutput->in(data);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
ChannelTyp SimpleLink<PipelineData, SharedData, GlobalData>::getLinkChannel() const
{
	return myLinkChannel;
}

}

#endif /* SIMPLE_LINK_H */
