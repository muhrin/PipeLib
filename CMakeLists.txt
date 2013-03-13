
cmake_minimum_required(VERSION 2.6)

project(Pipelib)

set(PIPELIB_VERSION_MAJOR 0)
set(PIPELIB_VERSION_MINOR 3)
set(PIPELIB_VERSION_PATCH 0)
set(PIPELIB_VERSION "${PIPELIB_VERSION_MAJOR}.${PIPELIB_VERSION_MINOR}.${PIPELIB_VERSION_PATCH}")
message(STATUS "Configuring Pipelib ${PIPELIB_VERSION}")


## CONFIGURATION SETTINGS ##############################

# Custom paths to libraries ####
set(BOOST_ROOT "" CACHE FILEPATH "Path to boost library")

# Build options ###
set(PIPELIB_ENABLE_TESTING FALSE CACHE BOOL "Build pipelib tests")



## FIND PACKAGES ########################################

# First add any user specified paths to the package search array
set(CMAKE_PREFIX_PATH
  "${BOOST_ROOT}"
)

#
# Boost #
# Disable auto-linking
add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost 1.36.0 REQUIRED)

## pipelib

set(pipelib_Header_Files__
  include/pipelib/Barrier.h
  include/pipelib/Block.h
  include/pipelib/BlockConnector.h
  include/pipelib/BlockIterator.h
  include/pipelib/LoaningPtr.h
  include/pipelib/pipelib.h
  include/pipelib/Pipe.h
  include/pipelib/PipelineState.h
  include/pipelib/PipeBlock.h
  include/pipelib/PipeEngine.h
  include/pipelib/PipeRunner.h
  include/pipelib/Pipeline.h
  include/pipelib/SimpleBarrier.h
  include/pipelib/SingleThreadedEngine.h
  include/pipelib/Sinks.h
  include/pipelib/StartBlock.h
  include/pipelib/Types.h
)
source_group("Header Files\\" FILES ${pipelib_Header_Files__})

## pipelib/detail

set(pipelib_Header_Files__detail
  include/pipelib/detail/Block.h
  include/pipelib/detail/BlockConnector.h
  include/pipelib/detail/BlockIterator.h
  include/pipelib/detail/LoaningPtr.h
  include/pipelib/detail/Pipe.h
  include/pipelib/detail/SimpleBarrier.h
  include/pipelib/detail/SingleThreadedEngine.h
)
source_group("Header Files\\detail" FILES ${pipelib_Header_Files__detail})

## pipelib/event

set(pipelib_Header_Files__event
  include/pipelib/event/EventSupport.h
  include/pipelib/event/PipeRunnerListener.h
  include/pipelib/event/PipeRunnerEvents.h
)
source_group("Header Files\\event" FILES ${pipelib_Header_Files__event})

## pipelib/event/detail

set(pipelib_Header_Files__event__detail
  include/pipelib/event/detail/PipeRunnerEvents.h
)
source_group("Header Files\\event\\detail" FILES ${pipelib_Header_Files__event__detail})

set(pipelib_Header_Files
  ${pipelib_Header_Files__}
  ${pipelib_Header_Files__detail}
  ${pipelib_Header_Files__event}
  ${pipelib_Header_Files__event__detail}
)

set(pipelib_Files
  ${pipelib_Header_Files}
  src/Dummy.cpp      # Have to have a cpp file to make add_library work
)

set(PIPELIB_INCLUDE_DIRS
  ${Boost_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  CACHE FILEPATH INTERNAL FORCE
)

include_directories(${PIPELIB_INCLUDE_DIRS})

add_library(pipelib ${pipelib_Files})



###########
## Tests ##
###########

if(PIPELIB_ENABLE_TESTING)
  add_subdirectory(tests)
endif(PIPELIB_ENABLE_TESTING)
