/*
 * PipeEngine.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_ENGINE_H
#define PIPE_ENGINE_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"


namespace pipelib {

template <typename T>
class LoanPtr;
template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeRunner;

template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeEngine
{
public:
  typedef StartBlock<PipelineData, SharedData, GlobalData> StartBlockType;
  typedef PipeRunner<PipelineData, SharedData, GlobalData> RunnerType;
  typedef LoanPtr<RunnerType> RunnerPtr;

  virtual void run(StartBlockType & startBlock) = 0;

  template <class StartBlockProxy>
  bool runProxy(StartBlockProxy & proxy);

  virtual RunnerPtr createRunner() = 0;
  virtual RunnerPtr createRunner(StartBlockType & subpipe) = 0;
};

template <typename PipelineData, typename SharedData, typename GlobalData>
template <class StartBlockProxy>
bool PipeEngine<PipelineData, SharedData, GlobalData>::runProxy(StartBlockProxy & proxy)
{
  StartBlockType * const startBlock = proxy.getStartBlock();
  if(!startBlock)
    return false;

  run(*startBlock);
  return true;
}

}

#endif /* PIPE_ENGINE_H */
