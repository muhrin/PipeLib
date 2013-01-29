/*
 * BlockConnector.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BLOCK_CONNECTOR_H
#define BLOCK_CONNECTOR_H

// INCLUDES /////////////////////////////////////////////
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include "pipelib/Pipeline.h"

namespace pipelib {

template <typename PipelineData, typename SharedData, typename GlobalData>
class Block;

template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeBlock;

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData,
  class OutputType = Block<PipelineData, SharedData, GlobalData>,
  class InputType = PipeBlock<PipelineData, SharedData, GlobalData>
>
class BlockConnector
{
public:
  BlockConnector(OutputType & block, const Channel channel);

  OutputType & operator |= (InputType & toConnect);

private:

  OutputType & myBlock;
  Channel myChannel;
};

}

#include "pipelib/detail/BlockConnector.h"

#endif /* BLOCK_CONNECTOR_H */
