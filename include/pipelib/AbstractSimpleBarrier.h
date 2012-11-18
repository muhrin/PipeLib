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
#include "pipelib/AbstractSimpleBlock.h"
#include "pipelib/Barrier.h"

// DEFINES ///////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {
	template <typename PipelineData, typename SharedData, typename GlobalData>
	class IPipeline;
}

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class AbstractSimpleBarrier :
	public AbstractSimpleBlock<PipelineData, SharedData, GlobalData>,
	public Barrier<PipelineData, SharedData, GlobalData>
{
public:

	AbstractSimpleBarrier() : Block<PipelineData, SharedData, GlobalData>("Abstract simple barrier") {}

	virtual void inserted(IPipeline<PipelineData, SharedData, GlobalData> & pipeline);

	virtual void removed();

private:
	typedef AbstractSimpleBlock<PipelineData, SharedData, GlobalData> MyAbsSimBlockTyp;
};

// IMPLEMENTATION ///////////////////////////

template <typename PipelineData, typename SharedData, typename GlobalData>
void AbstractSimpleBarrier<PipelineData, SharedData, GlobalData>::inserted(IPipeline<PipelineData, SharedData, GlobalData> & pipeline)
{
	// NOTE: Have to use this->(stuff) to access block members due to the way templates
	// work.  See: http://stackoverflow.com/questions/3829040/scope-problems-in-template-c
	PASSERT(!this->myPipeline);

	MyAbsSimBlockTyp::inserted(pipeline);
	this->myPipeline->registerBarrier(*this);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void AbstractSimpleBarrier<PipelineData, SharedData, GlobalData>::removed()
{
	PASSERT(this->myPipeline);

	this->myPipeline->deregisterBarrier(*this);
	MyAbsSimBlockTyp::removed();
}

}

#endif /* ABSTRACT_SIMPLE_BARRIER_H */
