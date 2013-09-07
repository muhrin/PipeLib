/*
 * BlockConnector.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BLOCK_CONNECTOR_DETAIL_H
#define BLOCK_CONNECTOR_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"
#include "pipelib/Types.h"

#include "pipelib/Block.h"

namespace pipelib {

template< typename Pipe, typename Shared, typename Global>
  ChannelConnector< Pipe, Shared, Global>::ChannelConnector(BlockType * const block,
      const Channel channel) :
      myBlock(block), myChannel(channel)
  {
  }

template< typename Pipe, typename Shared, typename Global>
  typename ChannelConnector< Pipe, Shared, Global>::BlockHandleType
  ChannelConnector< Pipe, Shared, Global>::operator ()(BlockHandleType to)
  {
    return myBlock->doConnect(to, myChannel);
  }

template< typename Pipe, typename Shared, typename Global>
  Connector< Pipe, Shared, Global>::Connector(BlockType * const block) :
      myBlock(block)
  {
  }

template< typename Pipe, typename Shared, typename Global>
  typename Connector< Pipe, Shared, Global>::BlockHandleType
  Connector< Pipe, Shared, Global>::operator ()(
      Connector< Pipe, Shared, Global>::BlockHandleType block)
  {
    return myBlock->doConnect(block, CHANNEL_DEFAULT);
  }

template< typename Pipe, typename Shared, typename Global>
  ChannelConnector< Pipe, Shared, Global>
  Connector< Pipe, Shared, Global>::operator [](const Channel channel)
  {
    return ChannelConnector<Pipe, Shared, Global>(myBlock, channel);
  }

}

#endif /* BLOCK_CONNECTOR_DETAIL_H */
