/*
 * StartBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef START_BLOCK_H
#define START_BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Block.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class StartBlock : public virtual Block<PipelineData, SharedData, GlobalData>
{
public:
	StartBlock() : Block<PipelineData, SharedData, GlobalData>("Start block") {}

	virtual void start() = 0;
};

}

#endif /* START_BLOCK_H */
