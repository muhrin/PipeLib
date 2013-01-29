/*
 * ManagedBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef MANAGED_BLOCK_H
#define MANAGED_BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"

#include <boost/shared_ptr.hpp>

#include "pipelib/BlockConnector.h"

namespace pipelib {

// FORWARD DECLARATIONS ////////////////////////////////////
template <typename PipelineData, typename SharedData, typename GlobalData>
class Block;
template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeBlock;
template <typename PipelineData, typename SharedData, typename GlobalData>
class StartBlock;

namespace managed {

template <typename PipelineData, typename SharedData, typename GlobalData>
class ManagedPipeline;

template <typename PipelineData, typename SharedData, typename GlobalData>
class ManagedBlock
{
  typedef BlockConnector<
    PipelineData,
    SharedData,
    GlobalData,
    ManagedBlock,
    ManagedBlock
  > ConnectorType;
  typedef StartBlock<PipelineData, SharedData, GlobalData> StartBlockType;
public:


  template <class BlockCreateType>
  static ManagedBlock create(PIPELIB_UNIQUE_PTR(BlockCreateType) blockPtr);

  ManagedBlock & operator |= (ManagedBlock & toConnect);
  ConnectorType operator[] (const Channel channel);
  void setOutput(ManagedBlock & output, const Channel channel = CHANNEL_DEFAULT);

  StartBlockType * getStartBlock();

private:
  typedef Block<PipelineData, SharedData, GlobalData> BlockType;
  typedef PipeBlock<PipelineData, SharedData, GlobalData> PipeBlockType;
  typedef ManagedPipeline<PipelineData, SharedData, GlobalData> ManagedPipeType;
  typedef ::boost::shared_ptr<ManagedPipeType> ManagedPipePtr;

  typedef typename ManagedPipeType::ManagedBlockHandle BlockHandle;

  ManagedBlock();
  explicit ManagedBlock(PipeBlockType * pipeBlock);
  explicit ManagedBlock(StartBlockType * startBlock);
  ManagedBlock(StartBlockType * startBlock, PipeBlockType * pipeBlock);

  ManagedPipePtr myManagedPipe;
  BlockHandle myBlockHandle;
};

} // namespace managed
} // namespace pipelib

#include "pipelib/managed/detail/ManagedBlock.h"

#endif /* MANAGED_BLOCK_H */
