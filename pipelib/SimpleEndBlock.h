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

#include "IFinishNotifiable.h"
#include "PipeBlock.h"


#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
class SimpleEndBlock :	public PipeBlock<DataType, GlobalDataType>
{
public:

	SimpleEndBlock(IFinishNotifiable<DataType> & notifiable);

	// From Block ////////////////////////

	virtual size_t getNumOutputs() const;
	virtual void setOutput(PipeBlock<DataType, GlobalDataType> & output, const ChannelTyp channel = CHANNEL_DEFAULT);
	virtual PipeBlock<DataType, GlobalDataType> * getOutput(const ChannelTyp channel = CHANNEL_DEFAULT) const;
	virtual void clearOutput(const ChannelTyp channel = CHANNEL_DEFAULT);

	// End from Block ////////////////////////

	// From PipeBlock ////////////////////////

	virtual void in(DataType & data);

	// End from PipeBlock ////////////////////

protected:
	IFinishNotifiable<DataType> &	myNotifiable;
};

// IMPLEMENTATION //////////////////////////////////////////

template <class DataType, class GlobalDataType>
SimpleEndBlock<DataType, GlobalDataType>::SimpleEndBlock(IFinishNotifiable<DataType> & notifiable):
Block<DataType, GlobalDataType>("Simple end block"),
myNotifiable(notifiable)
{}


template <class DataType, class GlobalDataType>
size_t SimpleEndBlock<DataType, GlobalDataType>::getNumOutputs() const
{
	return 0;	// End block - no outputs
}

template <class DataType, class GlobalDataType>
void SimpleEndBlock<DataType, GlobalDataType>::setOutput(PipeBlock<DataType, GlobalDataType> & output, const ChannelTyp channel)
{
	throw "Can't set output on end block";
}

template <class DataType, class GlobalDataType>
PipeBlock<DataType, GlobalDataType> *
SimpleEndBlock<DataType, GlobalDataType>::getOutput(const ChannelTyp channel) const
{
	return NULL;
}

template <class DataType, class GlobalDataType>
void SimpleEndBlock<DataType, GlobalDataType>::clearOutput(const ChannelTyp channel)
{
	throw "Cannot clear output on end block: it has no output";
}

template <class DataType, class GlobalDataType>
void SimpleEndBlock<DataType, GlobalDataType>::in(DataType & data)
{
	myNotifiable.dataFinished(data);
}

}

#endif /* SIMPLE_END_BLOCK_H */
