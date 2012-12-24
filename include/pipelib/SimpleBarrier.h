/*
 * SimpleBarrier.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SIMPLE_BARRIER_H
#define SIMPLE_BARRIER_H

// INCLUDES /////////////////////////////////////////////
#include <vector>

#include <pipelib/Barrier.h>

namespace pipelib {

template <typename PipelineData, typename SharedData, typename GlobalData>
class SimpleBarrier :	public Barrier<PipelineData, SharedData, GlobalData>
{
public:

	SimpleBarrier();

	virtual void in(PipelineData & data);

	virtual size_t release();

	virtual bool hasData() const;

protected:

	typedef ::std::vector<PipelineData *>	Buffer;

	Buffer	myBuffer;
};

}

#include "pipelib/detail/SimpleBarrier.h"

#endif /* SIMPLE_BARRIER_H */
