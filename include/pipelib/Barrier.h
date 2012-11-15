/*
 * Barrier.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BARRIER_H
#define BARRIER_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/PipeBlock.h"


// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class Barrier : public virtual PipeBlock<PipelineData, SharedData, GlobalData>
{
public:
	Barrier() : Block<PipelineData, SharedData, GlobalData>("Barrier") {}

	virtual size_t release() = 0;

	virtual bool hasData() const = 0;
};

// IMPLEMENTATION //////////////////////////


}

#endif /* BARRIER_H */
