/*
 * AbstractSimpleStartBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef ABSTRACT_SIMPLE_START_BLOCK_H
#define ABSTRACT_SIMPLE_START_BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include "Pipeline.h"

#include "StartBlock.h"

#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////


namespace pipelib {

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
class AbstractSimpleStartBlock : public StartBlock<DataType, GlobalDataType>
{
public:
	AbstractSimpleStartBlock();

	// From Block /////////////////////////////////////

	virtual size_t getNumOutputs() const;
	virtual PipeBlock<DataType, GlobalDataType> * getOutput(const size_t channel = CHANNEL_DEFAULT) const;
	virtual void setOutput(PipeBlock<DataType, GlobalDataType> & output, const size_t channel = CHANNEL_DEFAULT);
	virtual void clearOutput(const int channel = CHANNEL_DEFAULT);

	// End from Block ////////////////////////////////

protected:

	PipeBlock<DataType, GlobalDataType> *	myOutput;
};


// IMPLEMENTATION ///////////////////////////////////
template <class DataType, class GlobalDataType>
AbstractSimpleStartBlock<DataType, GlobalDataType>::AbstractSimpleStartBlock():
Block<DataType, GlobalDataType>("Abstract simple start block"),
myOutput(NULL)
{}

template <class DataType, class GlobalDataType>
size_t AbstractSimpleStartBlock<DataType, GlobalDataType>::getNumOutputs() const
{
	return 1;
}

template <class DataType, class GlobalDataType>
void AbstractSimpleStartBlock<DataType, GlobalDataType>::setOutput(PipeBlock<DataType, GlobalDataType> & output, const size_t channel)
{
	PASSERT(channel == 0);

	myOutput = &output;
}

template <class DataType, class GlobalDataType>
void AbstractSimpleStartBlock<DataType, GlobalDataType>::clearOutput(const int channel)
{
	if(!(channel == CHANNEL_DEFAULT || channel == CHANNEL_ALL))
	{
		throw "Trying to clear simple block output on non-deafult channel.";
	}

	myOutput = NULL;
}

template <class DataType, class GlobalDataType>
PipeBlock<DataType, GlobalDataType> * AbstractSimpleStartBlock<DataType, GlobalDataType>::getOutput(const size_t channel) const
{
	PASSERT(channel == 0);

	return myOutput;
}

}

#endif /* ABSTRACT_SIMPLE_START_BLOCK_H */
