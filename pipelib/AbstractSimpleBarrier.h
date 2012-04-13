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

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
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
	// NOTE: Have to use this->(stuff) to access block members due to the way templates
	// work.  See: http://stackoverflow.com/questions/3829040/scope-problems-in-template-c
	PASSERT(!this->myPipeline);

	MyAbsSimBlockTyp::inserted(pipeline);
	this->myPipeline->registerBarrier(*this);
}

template <class DataType, class GlobalDataType>
void AbstractSimpleBarrier<DataType, GlobalDataType>::removed()
{
	PASSERT(this->myPipeline);

	this->myPipeline->deregisterBarrier(*this);
	MyAbsSimBlockTyp::removed();
}

}

#endif /* ABSTRACT_SIMPLE_BARRIER_H */
