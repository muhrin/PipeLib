/*
 * ManagedPipeline.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef MANAGED_PIPELINE_DETAIL_H
#define MANAGED_PIPELINE_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"

#include <boost/static_assert.hpp>

#include "pipelib/PipeBlock.h"
#include "pipelib/StartBlock.h"

namespace pipelib {
namespace managed {


template <typename PipelineData, typename SharedData, typename GlobalData>
PipeManipulationOutcome::Value
ManagedPipeline<PipelineData, SharedData, GlobalData>::connect(
  ManagedBlockHandle & outBlock,
  ManagedBlockHandle & inBlock,
  const Channel channel)
{
  BlockType & out = outBlock->getBlock();
  if(channel >= static_cast<Channel>(out.getNumOutputs()))
    return PipeManipulationOutcome::ERROR__INVALID_CHANNEL;

  // Make sure nothing else is connected to this channel
  clearOutput(outBlock, channel);

  PipeBlockType * const in = inBlock->asPipeBlock();
  if(in == NULL)
    return PipeManipulationOutcome::ERROR__WRONG_INPUT_BLOCK_TYPE;

  out[channel] |= *in;
  inBlock->insertInput(*outBlock);

  return PipeManipulationOutcome::SUCCESS;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
PipeManipulationOutcome::Value
ManagedPipeline<PipelineData, SharedData, GlobalData>::clearOutput(
  ManagedBlockHandle & outBlock,
  const Channel channel)
{
  BlockType & out = outBlock->getBlock();
  // Make sure output is valid
  if(channel >= static_cast<Channel>(out.getNumOutputs()))
    return PipeManipulationOutcome::ERROR__INVALID_CHANNEL;

  // Make sure that the channel has an output
  if(!out.getOutput(channel))
    return PipeManipulationOutcome::WARNING__CHANNEL_NOT_SET;

  // Find the owner
  const BlocksMap::iterator it = myBlocks.find(&out);
  PIPELIB_ASSERT(it != myBlocks.end());

  // Clear the output
  clearOutput(it, channel);

  return PipeManipulationOutcome::SUCCESS;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
template <class BlockInsertType>
typename ManagedPipeline<PipelineData, SharedData, GlobalData>::ManagedBlockHandle
ManagedPipeline<PipelineData, SharedData, GlobalData>::insert(
  PIPELIB_UNIQUE_PTR(BlockInsertType) blockPtr
)
{
  const BlockType * key = blockPtr.get();
  BlockOwnerType & owner = *myBlocks.insert(
    key,
    BlockOwnerType::create(blockPtr).release()
  ).first->second;

  owner.setListener(this);
  return owner.createHandle();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
template <class BlockInsertType>
typename ManagedPipeline<PipelineData, SharedData, GlobalData>::ManagedBlockHandle
ManagedPipeline<PipelineData, SharedData, GlobalData>::insert(BlockInsertType * blockPtr)
{
  const BlockType * key = blockPtr;
  UniquePtr<BlockInsertType>::Type blockOwningPtr(blockPtr);
  BlockOwnerType & owner = *myBlocks.insert(
    key,
    BlockOwnerType::create(blockOwningPtr).release()
  ).first->second;

  owner.setListener(this);
  return owner.createHandle();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void ManagedPipeline<PipelineData, SharedData, GlobalData>::merge(ManagedPipeline & from)
{
  BOOST_FOREACH(BlocksMap::reference blockOwner, from.myBlocks)
  {
    (*blockOwner.second).setListener(this);
  }
  myBlocks.transfer(from.myBlocks);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void ManagedPipeline<PipelineData, SharedData, GlobalData>::clearOutput(const typename BlocksMap::iterator blockOwnerIt, const Channel channel)
{
  BlockType & block = blockOwnerIt->second->getBlock();
  const PipeBlockType * inBlock = block.getOutput(channel);
  PIPELIB_ASSERT(inBlock);

  // Find the owner of the input block and remove the input
  const BlocksMap::iterator inBlockIt = myBlocks.find(inBlock);
  PIPELIB_ASSERT(inBlockIt != myBlocks.end());

  BlockOwnerType & inBlockOwner = *inBlockIt->second;
  block.clearOutput(channel);
  inBlockOwner.eraseInput(*blockOwnerIt->second);

  // If the input block now has no inputs and no handles then erase it
  if(inBlockOwner.numInputs() == 0 && inBlockOwner.numHandles() == 0)
    erase(inBlockIt);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void ManagedPipeline<PipelineData, SharedData, GlobalData>::erase(const typename BlocksMap::iterator blockOwnerIt)
{
  const BlockType & block = *blockOwnerIt->first;

  // First disconnect any outputs
  for(size_t channel = 0; channel < static_cast<Channel>(block.getNumOutputs()); ++channel)
  {
    if(block.getOutput(channel))
      clearOutput(blockOwnerIt, channel);
  }

  // Now erase it
  myBlocks.erase(blockOwnerIt);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void ManagedPipeline<PipelineData, SharedData, GlobalData>::notifyNoHandles(BlockOwnerType & owner)
{
  const BlockType & block = owner.getBlock();
  BlocksMap::iterator blockIt = myBlocks.find(&block);
  PIPELIB_ASSERT(blockIt != myBlocks.end());

  if(owner.numInputs() == 0)
    erase(blockIt);
  else
  {
    bool hasHandles = false;
    for(BlockOwnerType::ConstPreorderReverseIterator it = owner.rBeginPreorder(), end = owner.rEndPreorder();
      it != end; ++it)
    {
      if(it->numHandles() > 0)
      {
        hasHandles = true;
        break;
      }
    }
    if(!hasHandles)
      erase(blockIt);
  }
}


} // namespace managed
} // namespace pipelib


#endif /* MANAGED_PIPELINE_DETAIL_H */
