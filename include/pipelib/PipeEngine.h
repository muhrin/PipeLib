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
class Pipe;
template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeRunner;

template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeEngine
{
public:
  typedef StartBlock<PipelineData, SharedData, GlobalData> StartBlockType;
  typedef Pipe<PipelineData, SharedData, GlobalData> PipeType;
  typedef PipeRunner<PipelineData, SharedData, GlobalData> RunnerType;
  typedef LoanPtr<RunnerType> RunnerPtr;

  virtual void run(PipeType & pipe) = 0;

  virtual RunnerPtr createRunner() = 0;
  virtual RunnerPtr createRunner(PipeType & subpipe) = 0;
};

}

#endif /* PIPE_ENGINE_H */
