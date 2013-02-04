/*
 * ManagedBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef MANAGED_BLOCK_DETAIL_H
#define MANAGED_BLOCK_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"

#include <boost/shared_ptr.hpp>

#include "pipelib/managed/ManagedPipeline.h"

namespace pipelib {
namespace managed {

template <typename PipelineData, typename SharedData, typename GlobalData>
template <typename BlockCreateType>
ManagedBlock<PipelineData, SharedData, GlobalData>
ManagedBlock<PipelineData, SharedData, GlobalData>::create(PIPELIB_UNIQUE_PTR(BlockCreateType) blockPtr)
{
  ManagedBlock block;
  block.myBlockHandle = block.myManagedPipe->insert(blockPtr);
  return block;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
ManagedBlock<PipelineData, SharedData, GlobalData> &
ManagedBlock<PipelineData, SharedData, GlobalData>::operator |=(ManagedBlock & toConnect)
{
  setOutput(toConnect);
  return *this;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename ManagedBlock<PipelineData, SharedData, GlobalData>::ConnectorType
ManagedBlock<PipelineData, SharedData, GlobalData>::operator [](const Channel channel)
{
  return ConnectorType(*this);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void ManagedBlock<PipelineData, SharedData, GlobalData>::setOutput(
  ManagedBlock & toConnect,
  const Channel channel)
{
  if(myManagedPipe.get() != toConnect.myManagedPipe.get())
  {
    myManagedPipe->merge(*toConnect.myManagedPipe);
    toConnect.myManagedPipe = myManagedPipe;
  }
  myManagedPipe->connect(myBlockHandle, toConnect.myBlockHandle, channel);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename ManagedBlock<PipelineData, SharedData, GlobalData>::StartBlockType *
ManagedBlock<PipelineData, SharedData, GlobalData>::getStartBlock()
{
  return myBlockHandle->asStartBlock();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
ManagedBlock<PipelineData, SharedData, GlobalData>::ManagedBlock():
myManagedPipe(new ManagedPipeType())
{}


} // namespace managed
} // namespace pipelib

#endif /* MANAGED_BLOCK_DETAIL_H */
