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

#include "pipelib/Block.h"

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData,
  class OutputType,
  class InputType
>
BlockConnector<PipelineData, SharedData, GlobalData, OutputType, InputType>::BlockConnector(
  OutputType & block,
  const Channel channel):
myBlock(block),
myChannel(channel)
{}

template <
  typename PipelineData,
  typename SharedData,
  typename GlobalData,
  class OutputType,
  class InputType
>
OutputType &
BlockConnector<PipelineData, SharedData, GlobalData, OutputType, InputType>::operator |=(InputType & toConnect)
{
  myBlock.setOutput(toConnect, myChannel);
  return myBlock;
}


}

#endif /* BLOCK_CONNECTOR_DETAIL_H */
