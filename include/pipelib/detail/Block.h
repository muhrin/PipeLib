/*
 * Block.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BLOCK_DETAIL_H
#define BLOCK_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <list>
#include <set>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include "pipelib/Pipeline.h"
#include "pipelib/BlockConnector.h"
#include "pipelib/BlockIterator.h"
#include "pipelib/PipeRunner.h"

namespace pipelib {

template <typename PipelineData, typename SharedData, typename GlobalData>
Block<PipelineData, SharedData, GlobalData>::Block(
  const ::std::string & name, const size_t numOutputs):
myName(name),
myNumOutputs(numOutputs),
myOutputs(new PipeBlockType *[numOutputs])
{
  // Null all outputs
  for(size_t i = 0; i < numOutputs; ++i)
    myOutputs[i] = NULL;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const ::std::string & Block<PipelineData, SharedData, GlobalData>::getName() const
{
	return myName;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
size_t Block<PipelineData, SharedData, GlobalData>::getNumOutputs() const
{
  return myNumOutputs;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
bool Block<PipelineData, SharedData, GlobalData>::clearOutput(const Channel channel)
{
  ASSERT(channel < getNumOutputs());

  const bool wasSet = myOutputs[channel];
  myOutputs[channel] = NULL;
  return wasSet;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename Block<PipelineData, SharedData, GlobalData>::PipeBlockType *
Block<PipelineData, SharedData, GlobalData>::getOutput(const Channel channel) const
{
  PIPELIB_ASSERT(channel < (int)getNumOutputs());

  return myOutputs[channel];
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename Block<PipelineData, SharedData, GlobalData>::OutputIterator
Block<PipelineData, SharedData, GlobalData>::beginOutputs()
{
  return myOutputs.get();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename Block<PipelineData, SharedData, GlobalData>::OutputIterator
Block<PipelineData, SharedData, GlobalData>::endOutputs()
{
  return myOutputs.get() + myNumOutputs;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename Block<PipelineData, SharedData, GlobalData>::ConstOutputIterator
Block<PipelineData, SharedData, GlobalData>::beginOutputs() const
{
  return myOutputs.get();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename Block<PipelineData, SharedData, GlobalData>::ConstOutputIterator
Block<PipelineData, SharedData, GlobalData>::endOutputs() const
{
  return myOutputs.get() + myNumOutputs;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
Block<PipelineData, SharedData, GlobalData> &
Block<PipelineData, SharedData, GlobalData>::operator |= (
  PipeBlockType & toConnect)
{
  setOutput(toConnect);
  return *this;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename Block<PipelineData, SharedData, GlobalData>::ConnectorType
Block<PipelineData, SharedData, GlobalData>::operator [] (const Channel channel)
{
  return ConnectorType(*this, channel);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename Block<PipelineData, SharedData, GlobalData>::PreorderIterator
Block<PipelineData, SharedData, GlobalData>::beginPreorder()
{
  return PreorderIterator(*this);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename Block<PipelineData, SharedData, GlobalData>::PreorderIterator
Block<PipelineData, SharedData, GlobalData>::endPreorder()
{
  return PreorderIterator();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename Block<PipelineData, SharedData, GlobalData>::ConstPreorderIterator
Block<PipelineData, SharedData, GlobalData>::beginPreorder() const
{
  ConstPreorderIterator(*this);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename Block<PipelineData, SharedData, GlobalData>::ConstPreorderIterator
Block<PipelineData, SharedData, GlobalData>::endPreorder() const
{
  ConstPreorderIterator();
}

/////////////////////////////////////
// PipeRunner messages //////////////

template <typename PipelineData, typename SharedData, typename GlobalData>
void Block<PipelineData, SharedData, GlobalData>::notifyAttached(RunnerSetupType & setup)
{
  // Pass on the message
  runnerAttached(setup);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void Block<PipelineData, SharedData, GlobalData>::notifyInitialising(RunnerAccessType & access)
{
  myRunner = &access;
  pipelineInitialising();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void Block<PipelineData, SharedData, GlobalData>::notifyInitialised()
{
  // Pass on the message
  pipelineInitialised();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void Block<PipelineData, SharedData, GlobalData>::notifyStarting()
{
  // Pass on the message
  pipelineStarting();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void Block<PipelineData, SharedData, GlobalData>::notifyFinishing()
{
  // Pass on the message
  pipelineFinishing();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void Block<PipelineData, SharedData, GlobalData>::notifyFinished(RunnerAccessType & access)
{
  PIPELIB_ASSERT(myRunner == &access);
  
  myRunner = NULL;
  // Pass on the message
  pipelineFinished();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void Block<PipelineData, SharedData, GlobalData>::notifyDetached()
{
  myRunner = NULL;
  // Pass on the message
  runnerDetached();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename Block<PipelineData, SharedData, GlobalData>::RunnerAccessType *
Block<PipelineData, SharedData, GlobalData>::getRunner()
{
	return myRunner;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const typename Block<PipelineData, SharedData, GlobalData>::RunnerAccessType *
Block<PipelineData, SharedData, GlobalData>::getRunner() const
{
	return myRunner;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void Block<PipelineData, SharedData, GlobalData>::out(
  PipelineData & data,
  const Channel channel) const
{
  // Tell the runner to pass on the data
  myRunner->out(data, *this, channel);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void Block<PipelineData, SharedData, GlobalData>::setOutput(
  PipeBlockType & output, const Channel channel)
{
  PIPELIB_ASSERT(channel < (int)getNumOutputs());

  myOutputs[channel] = &output;
}

}

#endif /* BLOCK_H */
