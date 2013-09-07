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
#include "pipelib/Pipeline.h"

#include "pipelib/BlockHandle.h"
#include "pipelib/Types.h"

namespace pipelib {

template <typename Pipe, typename Shared, typename Global>
class Block;

template <typename Pipe, typename Shared, typename Global>
class PipeBlock;

template < typename Pipe, typename Shared, typename Global>
class ChannelConnector
{
  typedef Block<Pipe, Shared, Global> BlockType;
  typedef typename BlockType::HandleType BlockHandleType;
public:
  ChannelConnector(BlockType * const block, const Channel channel);
  BlockHandleType operator ()(BlockHandleType to);
protected:
  BlockType * const myBlock;
  const Channel myChannel;
};

template < typename Pipe, typename Shared, typename Global>
class Connector
{
  typedef Block<Pipe, Shared, Global> BlockType;
  typedef typename BlockType::HandleType BlockHandleType;
public:
  Connector(BlockType * const block);
  BlockHandleType operator ()(BlockHandleType block);
  ChannelConnector<Pipe, Shared, Global> operator [](const Channel channel);
private:
  BlockType * const myBlock;
};

}

#include "pipelib/detail/BlockConnector.h"

#endif /* BLOCK_CONNECTOR_H */
