/*
 * SingleThreadedPipeline.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SINGLE_THREADED_PIPELINE_H
#define SINGLE_THREADED_PIPELINE_H

// INCLUDES /////////////////////////////////////////////
#include "Pipeline.h"

#include "AbstractLinkPipeline.h"
#include "SimpleEndBlock.h"
#include "SimpleLink.h"

#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
class SingleThreadedPipeline : public AbstractLinkPipeline<DataType, GlobalDataType>
{
public:

	virtual ~SingleThreadedPipeline();

	// From IPipeline ////////////////

	virtual SingleThreadedPipeline<DataType, GlobalDataType> & spawnChild();

	// End from IPipeline ////////////

protected:

	virtual ILink<DataType, GlobalDataType> * createLink();

	virtual void destroyLink(ILink<DataType, GlobalDataType> * const link);

private:

	typedef ::std::vector<SingleThreadedPipeline<DataType, GlobalDataType> * > ChildrenContainer;

	typedef ::std::vector<SimpleEndBlock<DataType, GlobalDataType> * > EndBlocksContainer;

	ChildrenContainer	myChildren;

	EndBlocksContainer	myEndBlocks;

};

// IMPLEMENTATION //////////////////////////////////////////

template <class DataType, class GlobalDataType>
SingleThreadedPipeline<DataType, GlobalDataType>::~SingleThreadedPipeline()
{
	// First delete my children
	for(typename ChildrenContainer::iterator it = myChildren.begin(), end = myChildren.end();
		it != end; ++it)
	{
		delete *it;
	}
	myChildren.clear();

	for(typename EndBlocksContainer::iterator it = myEndBlocks.begin(), end = myEndBlocks.end();
		it != end; ++it)
	{
		delete *it;
	}
	myEndBlocks.clear();
}

template <class DataType, class GlobalDataType>
SingleThreadedPipeline<DataType, GlobalDataType> &
SingleThreadedPipeline<DataType, GlobalDataType>::spawnChild()
{
	SingleThreadedPipeline<DataType, GlobalDataType> * child = new SingleThreadedPipeline<DataType, GlobalDataType>();
	myChildren.push_back(child);
	return *child;
}

template <class DataType, class GlobalDataType>
ILink<DataType, GlobalDataType> * SingleThreadedPipeline<DataType, GlobalDataType>::createLink()
{
	return new SimpleLink<DataType, GlobalDataType>(*this);
}

template <class DataType, class GlobalDataType>
void SingleThreadedPipeline<DataType, GlobalDataType>::destroyLink(ILink<DataType, GlobalDataType> * const link)
{
	PASSERT(link != NULL);

	delete link;
}

}

#endif /* SINGLE_THREADED_PIPELINE_H */
