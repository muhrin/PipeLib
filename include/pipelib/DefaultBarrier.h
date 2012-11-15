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
#include <pipelib/AbstractSimpleBarrier.h>

#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {
	template <typename PipelineData, typename SharedData, typename GlobalData>
	class Block;

	template <typename PipelineData, typename SharedData, typename GlobalData>
	class PipeBlock;
}

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class DefaultBarrier :	public AbstractSimpleBarrier<PipelineData, SharedData, GlobalData>
{
public:

	DefaultBarrier();

	virtual void in(PipelineData & data);

	virtual size_t release();

	virtual bool hasData() const;

protected:

	typedef ::std::vector<PipelineData *>	BufferVector;

	BufferVector	myBuffer;

};

// IMPLEMENTATION //////////////////////////////

template <typename PipelineData, typename SharedData, typename GlobalData>
DefaultBarrier<PipelineData, SharedData, GlobalData>::DefaultBarrier():
Block<PipelineData, SharedData, GlobalData>("Barrier block")
{}

template <typename PipelineData, typename SharedData, typename GlobalData>
void DefaultBarrier<PipelineData, SharedData, GlobalData>::in(PipelineData & data)
{
	myBuffer.push_back(&data);
	// Just return, don't pass it on:  This will be done upon release()
}

template <typename PipelineData, typename SharedData, typename GlobalData>
size_t DefaultBarrier<PipelineData, SharedData, GlobalData>::release()
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

template <typename PipelineData, typename SharedData, typename GlobalData>
bool DefaultBarrier<PipelineData, SharedData, GlobalData>::hasData() const
{
	return !myBuffer.empty();
}

}

#endif /* DEFAULT_BARRIER_H */
