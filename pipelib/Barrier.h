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
#include "PipeBlock.h"


// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
class Barrier : public virtual PipeBlock<DataType, GlobalDataType>
{
public:
	Barrier() : Block<DataType, GlobalDataType>("Barrier") {}

	virtual size_t release() = 0;

	virtual bool hasData() const = 0;
};

// IMPLEMENTATION //////////////////////////


}

#endif /* BARRIER_H */
