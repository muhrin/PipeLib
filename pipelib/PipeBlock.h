/*
 * PipeBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_BLOCK_H
#define PIPE_BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Block.h"

#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class PipeBlock : public virtual Block<PipelineData, SharedData, GlobalData>
{
public:

	PipeBlock(): Block<PipelineData, SharedData, GlobalData>("Pipe block") {}

	virtual void in(PipelineData & data) = 0;
};

}

#endif /* PIPE_BLOCK_H */
