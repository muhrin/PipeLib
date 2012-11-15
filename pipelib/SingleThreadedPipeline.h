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
#include "pipelib/Pipeline.h"

#include "pipelib/AbstractLinkPipeline.h"
#include "pipelib/SimpleEndBlock.h"
#include "pipelib/SimpleLink.h"

#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class SingleThreadedPipeline : public AbstractLinkPipeline<PipelineData, SharedData, GlobalData>
{
public:

  typedef SingleThreadedPipeline<PipelineData, SharedData, GlobalData> PipelineTyp;

  SingleThreadedPipeline();
	virtual ~SingleThreadedPipeline();

	// From IPipeline ////////////////

  virtual bool initialise();
	virtual SingleThreadedPipeline<PipelineData, SharedData, GlobalData> & spawnChild();

  virtual const PipelineTyp * getParent() const;

	// End from IPipeline ////////////

protected:

  SingleThreadedPipeline(PipelineTyp * const parent, GlobalData * const globalData);

	virtual ILink<PipelineData, SharedData, GlobalData> * createLink();

	virtual void destroyLink(ILink<PipelineData, SharedData, GlobalData> * const link);

private:

	typedef ::std::vector<SingleThreadedPipeline<PipelineData, SharedData, GlobalData> * > ChildrenContainer;

	typedef ::std::vector<SimpleEndBlock<PipelineData, SharedData, GlobalData> * > EndBlocksContainer;

  PipelineTyp * const myParent;

	ChildrenContainer   myChildren;

	EndBlocksContainer	myEndBlocks;

};

// IMPLEMENTATION //////////////////////////////////////////

template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedPipeline<PipelineData, SharedData, GlobalData>::SingleThreadedPipeline():
myParent(NULL)
{}

template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedPipeline<PipelineData, SharedData, GlobalData>::~SingleThreadedPipeline()
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

template <typename PipelineData, typename SharedData, typename GlobalData>
bool SingleThreadedPipeline<PipelineData, SharedData, GlobalData>::initialise()
{
  if(!AbstractLinkPipeline<PipelineData, SharedData, GlobalData>::Parent::initialise())
    return false;

  // Now initialise out children
  bool childrenInitialised = true;
  for(typename ChildrenContainer::iterator it = myChildren.begin(), end = myChildren.end();
    it != end; ++it)
  {
    if(!(*it)->initialise())
    {
      childrenInitialised = false;
      break;
    }
  }

  return childrenInitialised;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedPipeline<PipelineData, SharedData, GlobalData> &
SingleThreadedPipeline<PipelineData, SharedData, GlobalData>::spawnChild()
{
	SingleThreadedPipeline<PipelineData, SharedData, GlobalData> * child =
    new SingleThreadedPipeline<PipelineData, SharedData, GlobalData>(
    this,
    AbstractLinkPipeline<PipelineData, SharedData, GlobalData>::myGlobalData);
	myChildren.push_back(child);
	return *child;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const SingleThreadedPipeline<PipelineData, SharedData, GlobalData> *
SingleThreadedPipeline<PipelineData, SharedData, GlobalData>::getParent() const
{
  return myParent;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
ILink<PipelineData, SharedData, GlobalData> * SingleThreadedPipeline<PipelineData, SharedData, GlobalData>::createLink()
{
	return new SimpleLink<PipelineData, SharedData, GlobalData>(*this);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
SingleThreadedPipeline<PipelineData, SharedData, GlobalData>::SingleThreadedPipeline(
  SingleThreadedPipeline<PipelineData, SharedData, GlobalData> * const parent,
  GlobalData * const globalData):
AbstractLinkPipeline<PipelineData, SharedData, GlobalData>(globalData),
myParent(parent)
{}

template <typename PipelineData, typename SharedData, typename GlobalData>
void SingleThreadedPipeline<PipelineData, SharedData, GlobalData>::destroyLink(ILink<PipelineData, SharedData, GlobalData> * const link)
{
	PASSERT(link != NULL);

	delete link;
}

}

#endif /* SINGLE_THREADED_PIPELINE_H */
