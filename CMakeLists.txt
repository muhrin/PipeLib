
cmake_minimum_required(VERSION 2.6)

set(pipelib_Header_Files__
  include/pipelib/AbstractLinkPipeline.h
  include/pipelib/AbstractPipeline.h
  include/pipelib/AbstractSimpleBarrier.h
  include/pipelib/AbstractSimpleBlock.h
  include/pipelib/AbstractSimpleStartBlock.h
  include/pipelib/Barrier.h
  include/pipelib/Block.h
  include/pipelib/DefaultBarrier.h
  include/pipelib/DirectPipeline.h
  include/pipelib/EmptyGlobalData.h
  include/pipelib/IDataSink.h
  include/pipelib/IFinishNotifiable.h
  include/pipelib/ILink.h
  include/pipelib/IPipeline.h
  include/pipelib/PipelineState.h
  include/pipelib/PipeBlock.h
  include/pipelib/Pipeline.h
  include/pipelib/SimpleEndBlock.h
  include/pipelib/SimpleLink.h
  include/pipelib/SingleThreadedPipeline.h
  include/pipelib/StartBlock.h
)
source_group("Header Files\\" FILES ${pipelib_Header_Files__})

set(pipelib_Header_Files__event
  include/pipelib/event/EventSupport.h
  include/pipelib/event/IPipeListener.h
  include/pipelib/event/PipeStateChanged.h
)
source_group("Header Files\\event" FILES ${pipelib_Header_Files__event})

set(pipelib_Header_Files
  ${pipelib_Header_Files__}
  ${pipelib_Header_Files__event}
)

set(pipelib_Files
  ${pipelib_Header_Files}
  src/Dummy.cpp      # Have to have a cpp file to make add_library work
)

include_directories(include)

add_library(pipelib ${pipelib_Files})
