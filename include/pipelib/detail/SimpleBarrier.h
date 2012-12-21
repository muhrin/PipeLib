/*
 * SimpleBarrier.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SIMPLE_BARRIER_DETAIL_H
#define SIMPLE_BARRIER_DETAIL_H

// INCLUDES /////////////////////////////////////////////

#include <boost/foreach.hpp>

namespace pipelib {


template <typename PipelineData, typename SharedData, typename GlobalData>
SimpleBarrier<PipelineData, SharedData, GlobalData>::SimpleBarrier():
Block<PipelineData, SharedData, GlobalData>("Simple barrier")
{}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SimpleBarrier<PipelineData, SharedData, GlobalData>::in(PipelineData & data)
{
	myBuffer.push_back(&data);
	// Just return, don't pass it on:  This will be done upon release()
}

template <typename PipelineData, typename SharedData, typename GlobalData>
size_t SimpleBarrier<PipelineData, SharedData, GlobalData>::release()
{
	const size_t bufferSize = myBuffer.size();
  BOOST_FOREACH(PipelineData * const data, myBuffer)
	{
    out(*data);
	}

	// Done our job, so clear the buffer
	myBuffer.clear();

	return bufferSize;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
bool SimpleBarrier<PipelineData, SharedData, GlobalData>::hasData() const
{
	return !myBuffer.empty();
}

}

#endif /* SIMPLE_BARRIER_DETAIL_H */
