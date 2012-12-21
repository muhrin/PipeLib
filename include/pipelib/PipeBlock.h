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

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeBlock : public virtual Block<PipelineData, SharedData, GlobalData>
{
protected:
  typedef Block<PipelineData, SharedData, GlobalData> BlockType;

public:
  typedef BlockConnector<PipelineData, SharedData, GlobalData, PipeBlock> ConnectorType;

  PipeBlock(const size_t numOutputs = 1): BlockType("Pipe block", numOutputs) {}

  // Hide Block's operator |= so that we can return a PipeBlock reference
  PipeBlock & operator |= (PipeBlock & toConnect)
  {
    BlockType::operator |= (toConnect);
    return *this;
  }

  ConnectorType operator[] (const Channel channel)
  {
    return ConnectorType(*this, channel);
  }

	virtual void in(PipelineData & data) = 0;
};

}

#endif /* PIPE_BLOCK_H */
