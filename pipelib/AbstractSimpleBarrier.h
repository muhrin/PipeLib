/*
 * AbstractSimpleBarrier.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef ABSTRACT_SIMPLE_BARRIER_H
#define ABSTRACT_SIMPLE_BARRIER_H

// INCLUDES /////////////////////////////////////////////
#include "AbstractSimpleBlock.h"
#include "Barrier.h"

// DEFINES ///////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {
	template <class DataType, class GlobalDataType>
	class IPipeline;
}

namespace pipelib {

template <class DataType, class GlobalDataType = EmptyGlobalData>
class AbstractSimpleBarrier :
	public AbstractSimpleBlock<DataType, GlobalDataType>,
	public Barrier<DataType, GlobalDataType>
{
public:

	AbstractSimpleBarrier() : Block<DataType, GlobalDataType>("Abstract simple barrier") {}

	virtual void inserted(IPipeline<DataType, GlobalDataType> & pipeline);

	virtual void removed();

private:
	typedef AbstractSimpleBlock<DataType, GlobalDataType> MyAbsSimBlockTyp;
};

// IMPLEMENTATION ///////////////////////////

template <class DataType, class GlobalDataType>
void AbstractSimpleBarrier<DataType, GlobalDataType>::inserted(IPipeline<DataType, GlobalDataType> & pipeline)
{
	PASSERT(!myPipeline);

	MyAbsSimBlockTyp::inserted(pipeline);
	myPipeline->registerBarrier(*this);
}

template <class DataType, class GlobalDataType>
void AbstractSimpleBarrier<DataType, GlobalDataType>::removed()
{
	PASSERT(myPipeline);

	myPipeline->deregisterBarrier(*this);
	MyAbsSimBlockTyp::removed();
}

}

#endif /* ABSTRACT_SIMPLE_BARRIER_H */
