/*
 * DefaultBarrier.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef DEFAULT_BARRIER_H
#define DEFAULT_BARRIER_H

// INCLUDES /////////////////////////////////////////////
#include "AbstractSimpleBarrier.h"

#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {
	template <class DataType, class GlobalDataType>
	class Block;

	template <class DataType, class GlobalDataType>
	class PipeBlock;
}

namespace pipelib {

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
class DefaultBarrier :	public AbstractSimpleBarrier<DataType, GlobalDataType>
{
public:

	DefaultBarrier();

	virtual void in(DataType & data);

	virtual size_t release();

	virtual bool hasData() const;

protected:

	typedef ::std::vector<DataType *>	BufferVector;

	BufferVector	myBuffer;

};

// IMPLEMENTATION //////////////////////////////

template <class DataType, class GlobalDataType>
DefaultBarrier<DataType, GlobalDataType>::DefaultBarrier():
Block<DataType, GlobalDataType>("Barrier block")
{}

template <class DataType, class GlobalDataType>
void DefaultBarrier<DataType, GlobalDataType>::in(DataType & data)
{
	myBuffer.push_back(&data);
	// Just return, don't pass it on:  This will be done upon release()
}

template <class DataType, class GlobalDataType>
size_t DefaultBarrier<DataType, GlobalDataType>::release()
{
	const size_t bufferSize = myBuffer.size();
	for(typename BufferVector::iterator it = myBuffer.begin(), end = myBuffer.end();
		it != end; ++it)
	{
		// Have to use this-> for template reasons
		this->myOutput->in(**it);
	}

	// Done our job, so clear the buffer
	myBuffer.clear();

	return bufferSize;
}

template <class DataType, class GlobalDataType>
bool DefaultBarrier<DataType, GlobalDataType>::hasData() const
{
	return !myBuffer.empty();
}

}

#endif /* DEFAULT_BARRIER_H */
