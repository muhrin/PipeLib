/*
 * Pipe.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_DETAIL_H
#define PIPE_DETAIL_H

// INCLUDES /////////////////////////////////////////////


// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
Pipe<PipelineData, SharedData, GlobalData>::Pipe():
myStartBlock(NULL)
{}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
template <typename T>
T * Pipe<PipelineData, SharedData, GlobalData>::addBlock(T * const block)
{
  PIPELIB_ASSERT(block);

  myBlocks.push_back(block);
  return block;
}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
void Pipe<PipelineData, SharedData, GlobalData>::connect(
  BlockType * block1, PipeBlockType * block2, const Channel channel)
{
  PIPELIB_ASSERT(block1);
  PIPELIB_ASSERT(block2);

  block1->setOutput(*block2, channel);
}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
bool Pipe<PipelineData, SharedData, GlobalData>::clearOutput(
  BlockType * block, const Channel channel)
{
  PIPELIB_ASSERT(block);

  if(!block->getOutput(channel()))
    return false;

  block->clearOutput(channel);
  return true;
}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
void Pipe<PipelineData, SharedData, GlobalData>::setStartBlock(StartBlockType * startBlock)
{
  PIPELIB_ASSERT(startBlock);

  myStartBlock = startBlock;
}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
typename Pipe<PipelineData, SharedData, GlobalData>::StartBlockType *
Pipe<PipelineData, SharedData, GlobalData>::getStartBlock()
{
  return myStartBlock;
}


template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
const typename Pipe<PipelineData, SharedData, GlobalData>::StartBlockType *
Pipe<PipelineData, SharedData, GlobalData>::getStartBlock() const
{
  return myStartBlock;
}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
void Pipe<PipelineData, SharedData, GlobalData>::notifyAttached(RunnerSetupType & runnerSetup)
{
  // Tell all the blocks about the detachment
  for(typename BlockType::PreorderIterator it = getStartBlock()->beginPreorder(),
    end = getStartBlock()->endPreorder(); it != end; ++it)
  {
    it->notifyAttached(runnerSetup);
  }
}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
void Pipe<PipelineData, SharedData, GlobalData>::notifyInitialising(RunnerAccessType & access)
{
  // Tell all the blocks about the detachment
  for(typename BlockType::PreorderIterator it = getStartBlock()->beginPreorder(),
    end = getStartBlock()->endPreorder(); it != end; ++it)
  {
    it->notifyInitialising(access);
  }
}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
void Pipe<PipelineData, SharedData, GlobalData>::notifyInitialised()
{
  // Tell all the blocks about the detachment
  for(typename BlockType::PreorderIterator it = getStartBlock()->beginPreorder(),
    end = getStartBlock()->endPreorder(); it != end; ++it)
  {
    it->notifyInitialised();
  }
}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
void Pipe<PipelineData, SharedData, GlobalData>::notifyStarting()
{
  // Tell all the blocks about the detachment
  for(typename BlockType::PreorderIterator it = getStartBlock()->beginPreorder(),
    end = getStartBlock()->endPreorder(); it != end; ++it)
  {
    it->notifyStarting();
  }
}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
void Pipe<PipelineData, SharedData, GlobalData>::notifyFinishing()
{
  // Tell all the blocks about the detachment
  for(typename BlockType::PreorderIterator it = getStartBlock()->beginPreorder(),
    end = getStartBlock()->endPreorder(); it != end; ++it)
  {
    it->notifyFinishing();
  }
}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
void Pipe<PipelineData, SharedData, GlobalData>::notifyFinished(RunnerAccessType & access)
{
  // Tell all the blocks about the detachment
  for(typename BlockType::PreorderIterator it = getStartBlock()->beginPreorder(),
    end = getStartBlock()->endPreorder(); it != end; ++it)
  {
    it->notifyFinished(access);
  }
}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
void Pipe<PipelineData, SharedData, GlobalData>::notifyDetached()
{
  // Tell all the blocks about the detachment
  for(typename BlockType::PreorderIterator it = getStartBlock()->beginPreorder(),
    end = getStartBlock()->endPreorder(); it != end; ++it)
  {
    it->notifyDetached();
  }
}

}

#endif /* PIPE_DETAIL_H */
