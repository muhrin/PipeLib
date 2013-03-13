/*
 * Pipe.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_H
#define PIPE_H

// INCLUDES /////////////////////////////////////////////
#include <string>

#include <boost/ptr_container/ptr_vector.hpp>

#include "pipelib/Pipeline.h"
#include "pipelib/PipeRunner.h"
#include "pipelib/Block.h"

namespace pipelib {

// FORWARD DECLARATIONS ////////////////////////////////////

template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeRunner;

template <typename PipelineData, typename SharedData, typename GlobalData>
class StartBlock;

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
class Pipe
{
public:
  typedef PipelineData PipelineDataType;
  typedef SharedData SharedDataType;
  typedef GlobalData GlobalDataType;

private:
  typedef typename PipeRunnerTypes<Pipe>::Runner PipeRunnerType;
  typedef typename PipeRunnerTypes<Pipe>::Access RunnerAccessType;
  typedef typename PipeRunnerTypes<Pipe>::Setup RunnerSetupType;

public:

  typedef Block<PipelineData, SharedData, GlobalData> BlockType;
  typedef PipeBlock<PipelineData, SharedData, GlobalData> PipeBlockType;
  typedef StartBlock<PipelineData, SharedData, GlobalData> StartBlockType;

  Pipe();
  template <typename T>
  T * addBlock(T * const block);
  void connect(BlockType * block1, PipeBlockType * block2, const Channel channel = CHANNEL_DEFAULT);
  bool clearOutput(BlockType * block, const Channel channel = CHANNEL_DEFAULT);
  void setStartBlock(StartBlockType * block);

  StartBlockType * getStartBlock();
  const StartBlockType * getStartBlock() const;

private:
  typedef ::boost::ptr_vector<BlockType> BlockContainer;

  void notifyAttached(RunnerSetupType & setup);
  void notifyInitialising(RunnerAccessType & access);
  void notifyInitialised();
  void notifyStarting();
  void notifyFinishing();
  void notifyFinished(RunnerAccessType & access);
  void notifyDetached();

  StartBlockType * myStartBlock;
  BlockContainer myBlocks;

  friend class PipeRunner<PipelineData, SharedData, GlobalData>;
};

}

#include "pipelib/detail/Pipe.h"

#endif /* PIPE_H */

