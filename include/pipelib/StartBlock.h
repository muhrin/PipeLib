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

template <typename PipelineData, typename SharedData, typename GlobalData>
class StartBlock : public virtual Block<PipelineData, SharedData, GlobalData>
{
protected:
  typedef Block<PipelineData, SharedData, GlobalData> BlockType;

public:
  StartBlock(const size_t numOutputs = 1): BlockType("Start block", numOutputs) {}

	virtual void start() = 0;

  virtual StartBlock * asStartBlock() { return this; }
  virtual const StartBlock * asStartBlock() const { return this; }
};

}

#endif /* START_BLOCK_H */
