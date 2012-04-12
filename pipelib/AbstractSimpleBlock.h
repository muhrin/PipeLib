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
#include "Pipeline.h"
#include "PipeBlock.h"

#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
class AbstractSimpleBlock :	public virtual PipeBlock<DataType, GlobalDataType>
{
public:
	AbstractSimpleBlock();
	
	// From Block /////////////////////////////

	virtual size_t getNumOutputs() const;
	virtual PipeBlock<DataType, GlobalDataType> * getOutput(const size_t channel = CHANNEL_DEFAULT) const;
	virtual void setOutput(PipeBlock<DataType, GlobalDataType> & output, const size_t channel = CHANNEL_DEFAULT);
	virtual void clearOutput(const int channel = CHANNEL_DEFAULT);
	
	// End From Block ///////////////////////////

protected:

	PipeBlock<DataType, GlobalDataType> *	myOutput;
};


// IMPLEMENTATION ///////////////////////////////////
template <class DataType, class GlobalDataType>
AbstractSimpleBlock<DataType, GlobalDataType>::AbstractSimpleBlock():
Block("Abstract simple block"),
myOutput(NULL)
{}

template <class DataType, class GlobalDataType>
size_t AbstractSimpleBlock<DataType, GlobalDataType>::getNumOutputs() const
{
	return 1;
}

template <class DataType, class GlobalDataType>
void AbstractSimpleBlock<DataType, GlobalDataType>::setOutput(PipeBlock<DataType, GlobalDataType> & output, const size_t channel)
{
	PASSERT(channel == 0);

	myOutput = &output;
}

template <class DataType, class GlobalDataType>
void AbstractSimpleBlock<DataType, GlobalDataType>::clearOutput(const int channel)
{
	if(!(channel == CHANNEL_DEFAULT || channel == CHANNEL_ALL))
	{
		throw "Trying to clear simple block output on non-deafult channel.";
	}

	myOutput = NULL;
}

template <class DataType, class GlobalDataType>
PipeBlock<DataType, GlobalDataType> * AbstractSimpleBlock<DataType, GlobalDataType>::getOutput(const size_t channel) const
{
	PASSERT(channel == 0);

	return myOutput;
}

}

#endif /* ABSTRACT_SIMPLE_BLOCK_H */
