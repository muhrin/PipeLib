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
  class ConnectingType = Block<PipelineData, SharedData, GlobalData>
>
class BlockConnector
{
  typedef Block<PipelineData, SharedData, GlobalData> BlockType;

  BOOST_STATIC_ASSERT((::boost::is_base_of<ConnectingType, BlockType>::value));
public:

  typedef PipeBlock<PipelineData, SharedData, GlobalData> PipeBlockType;

  BlockConnector(ConnectingType & block, const Channel channel);

  ConnectingType & operator |= (PipeBlockType & toConnect);

private:

  ConnectingType & myBlock;
  Channel myChannel;
};

}

#include "pipelib/detail/BlockConnector.h"

#endif /* BLOCK_CONNECTOR_H */
