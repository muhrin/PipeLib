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

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData,
  typename PipelineData & = PipelineData &,
  typename SharedData & = SharedData &,
  typename GlobalData & = GlobalData &
>
class PipeEngine
{
public:

  typedef Block<PipelineData, SharedData, GlobalData> BlockType;
  typedef PipeBlock<PipelineData, SharedData, GlobalData> PipeBlockType;
  typedef StartBlock<PipelineData, SharedData, GlobalData> StartBlockType;

  virtual void run(StartBlockType & startBlock) = 0;

};

}

#endif /* PIPE_ENGINE_H */
