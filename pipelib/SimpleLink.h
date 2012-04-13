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
#include "Pipeline.h"

#include "AbstractLinkPipeline.h"
#include "AbstractSimpleBlock.h"
#include "ILink.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
class SimpleLink : public AbstractSimpleBlock<DataType, GlobalDataType>, public ILink<DataType, GlobalDataType>
{
public:
	SimpleLink(AbstractLinkPipeline<DataType, GlobalDataType> & pipeline);


	// From ILink ////////////////////////////////////////////////

	virtual void link(
		Block<DataType, GlobalDataType> & input,
		PipeBlock<DataType, GlobalDataType> & output,
		const ChannelTyp outChannel = CHANNEL_DEFAULT);

	virtual void unlink();

	virtual Block<DataType, GlobalDataType> & getInput() const;

	ChannelTyp getLinkChannel() const;

	// End ILink //////////////////////////////////////////////////

	// From PipeBlock ////////////////////////////////////////////

	virtual void in(DataType & data);

	// End PipeBlock /////////////////////////////////////////////

protected:

	/** The pipeline that this link comminucates with. */
	AbstractLinkPipeline<DataType, GlobalDataType> &	myLinkPipeline;

	/** The blocks that is attached to the input of this link. */
	Block<DataType, GlobalDataType> *		myInput;

	/** The output channel of the outputter that this link takes as an input */
	ChannelTyp						myLinkChannel;
};

// IMPLEMENTATION /////////////////////////

template <class DataType, class GlobalDataType>
SimpleLink<DataType, GlobalDataType>::SimpleLink(AbstractLinkPipeline<DataType, GlobalDataType> & pipeline):
Block<DataType, GlobalDataType>("Simple link"),
myLinkPipeline(pipeline),
myInput(NULL),
myLinkChannel(CHANNEL_NONE)
{
}

template <class DataType, class GlobalDataType>
void SimpleLink<DataType, GlobalDataType>::link(
	Block<DataType, GlobalDataType> & input,
	PipeBlock<DataType, GlobalDataType> & output,
	const ChannelTyp outChannel)
{
	myInput = &input;
	myLinkChannel = outChannel;
	myInput->setOutput(*this, myLinkChannel);
	setOutput(output);
}

template <class DataType, class GlobalDataType>
void SimpleLink<DataType, GlobalDataType>::unlink()
{
    // Have to use this-> as we're calling this in a template base class
	this->clearOutput();
	myInput->clearOutput(myLinkChannel);
	myLinkChannel = CHANNEL_NONE;
	myInput = NULL;
}

template <class DataType, class GlobalDataType>
Block<DataType, GlobalDataType> & SimpleLink<DataType, GlobalDataType>::getInput() const
{
	return *myInput;
}

template <class DataType, class GlobalDataType>
void SimpleLink<DataType, GlobalDataType>::in(DataType & data)
{
	// Tell the pipeline that this link is passing data
	myLinkPipeline.linkCallback(*this, data);

	// Pass the data on to the next block
	this->myOutput->in(data);
}

template <class DataType, class GlobalDataType>
ChannelTyp SimpleLink<DataType, GlobalDataType>::getLinkChannel() const
{
	return myLinkChannel;
}

}

#endif /* SIMPLE_LINK_H */
