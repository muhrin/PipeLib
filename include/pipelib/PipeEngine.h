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
  virtual RunnerPtr createRunner() = 0;
  virtual RunnerPtr createRunner(StartBlockType & subpipe) = 0;
};

}

#endif /* PIPE_ENGINE_H */
