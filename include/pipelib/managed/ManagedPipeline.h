/*
 * ManagedPipeline.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef MANAGED_PIPELINE_H
#define MANAGED_PIPELINE_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include "pipelib/managed/detail/BlockOwner.h"

namespace pipelib {

// FORWARD DECLARATIONS ////////////////////////////////////
template <typename PipelineData, typename SharedData, typename GlobalData>
class Block;
template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeBlock;
template <typename PipelineData, typename SharedData, typename GlobalData>
class StartBlock;

namespace managed {

struct PipeManipulationOutcome
{
  enum Value
  {
    SUCCESS,
    //
    // Warnings
    WARNING__CHANNEL_NOT_SET,

    //
    // Errors

    // General
    ERROR__INVALID_CHANNEL,
    // Connection
    ERROR__WRONG_INPUT_BLOCK_TYPE,
    // Disconnection
    ERROR__CHANNEL_NOT_SET
  };
};

template <typename PipelineData, typename SharedData, typename GlobalData>
class ManagedPipeline :
  private detail::IBlockOwnerListener<PipelineData, SharedData, GlobalData>,
  private ::boost::noncopyable
{
public:
  typedef detail::BlockOwner<PipelineData, SharedData, GlobalData> BlockOwnerType;
  typedef typename BlockOwnerType::HandleType ManagedBlockHandle;

  PipeManipulationOutcome::Value connect(
    ManagedBlockHandle & outBlock,
    ManagedBlockHandle & inBlock,
    const Channel channel = CHANNEL_DEFAULT
  );

  PipeManipulationOutcome::Value clearOutput(
    ManagedBlockHandle & outBlock,
    const Channel channel = CHANNEL_DEFAULT
  );

  template <class BlockInsertType>
  ManagedBlockHandle insert(PIPELIB_UNIQUE_PTR(BlockInsertType) blockPtr);

  template <class BlockInsertType>
  ManagedBlockHandle insert(BlockInsertType * const blockPtr);

  void merge(ManagedPipeline & from);
  
private:
  typedef Block<PipelineData, SharedData, GlobalData> BlockType;
  typedef PipeBlock<PipelineData, SharedData, GlobalData> PipeBlockType;
  typedef StartBlock<PipelineData, SharedData, GlobalData> StartBlockType;
  typedef ::boost::ptr_map<const BlockType *, BlockOwnerType> BlocksMap;

  void clearOutput(const typename BlocksMap::iterator blockOwnerIt, const Channel channel);
  void erase(const typename BlocksMap::iterator blockOwnerIt);

  // From IBlockOwnerListener
  virtual void notifyNoHandles(BlockOwnerType & owner);
  // End From IBlockOwnerListener

  BlocksMap myBlocks;

  friend class detail::BlockOwner<PipelineData, SharedData, GlobalData>;
};

} // namespace managed
} // namespace pipelib

#include "pipelib/managed/detail/ManagedPipeline.h"

#endif /* BLOCK_OWNER_H */
