
cmake_minimum_required(VERSION 2.6)

project(Pipelib)

set(PIPELIB_VERSION_MAJOR 0)
set(PIPELIB_VERSION_MINOR 5)
set(PIPELIB_VERSION_PATCH 1)
set(PIPELIB_VERSION "${PIPELIB_VERSION_MAJOR}.${PIPELIB_VERSION_MINOR}.${PIPELIB_VERSION_PATCH}")
message(STATUS "Configuring Pipelib ${PIPELIB_VERSION}")


## CONFIGURATION SETTINGS ##############################

# Custom paths to libraries ####
set(BOOST_ROOT "" CACHE FILEPATH "Path to boost library")

# Build options ###
set(PIPELIB_ENABLE_TESTING FALSE CACHE BOOL "Build pipelib tests")

set(PIPELIB_USE_BOOST_THREAD FALSE CACHE BOOL "Enable use of boost threads.")

configure_file(
  "${PROJECT_SOURCE_DIR}/include/pipelib/PipelibConfig.h.in"
  "${PROJECT_BINARY_DIR}/include/pipelib/PipelibConfig.h"
)

## FIND PACKAGES ########################################

#
# Boost #
# Disable auto-linking
add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost 1.36.0 REQUIRED)

if(PIPELIB_USE_BOOST_THREAD)
  set(Boost_USE_MULTITHREADED ON)
  find_package(Boost 1.36.0 COMPONENTS system thread REQUIRED)
  find_package(Threads REQUIRED)
endif(PIPELIB_USE_BOOST_THREAD)

## pipelib

set(pipelib_Header_Files__
#  ${PROJECT_BINARY_DIR}/include/pipelib/PipelibConfig.h
  include/pipelib/Barrier.h
  include/pipelib/Block.h
  include/pipelib/BlockHandle.h
  include/pipelib/BlockConnector.h
  include/pipelib/BlockHandle.h
  include/pipelib/BlockIterator.h
  include/pipelib/BoostThreadEngine.h
  include/pipelib/pipelib.h
  include/pipelib/PipelineState.h
  include/pipelib/PipeBlock.h
  include/pipelib/PipeEngine.h
  include/pipelib/Pipeline.h
  include/pipelib/SerialEngine.h
  include/pipelib/SimpleBarrier.h
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
  include/pipelib/detail/BoostThreadEngine.h
  include/pipelib/detail/PipeEngine.h
  include/pipelib/detail/SerialEngine.h
  include/pipelib/detail/SimpleBarrier.h
)
source_group("Header Files\\detail" FILES ${pipelib_Header_Files__detail})

## pipelib/event

set(pipelib_Header_Files__event
  include/pipelib/event/EventSupport.h
  include/pipelib/event/PipeEngineListener.h
  include/pipelib/event/PipeEngineEvents.h
  include/pipelib/event/PipeEngineStateChanged.h
)
source_group("Header Files\\event" FILES ${pipelib_Header_Files__event})

## pipelib/event/detail

set(pipelib_Header_Files__event__detail
  include/pipelib/event/detail/PipeEngineEvents.h
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
  ${PROJECT_BINARY_DIR}/include # So that we find PipelibConfig.h
  ${PROJECT_SOURCE_DIR}/include
  CACHE FILEPATH INTERNAL FORCE
)

include_directories(${PIPELIB_INCLUDE_DIRS})

add_library(pipelib ${pipelib_Files})
if(PIPELIB_USE_BOOST_THREAD)
target_link_libraries(pipelib
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
)
endif(PIPELIB_USE_BOOST_THREAD)

# Save our include dirs to this variable so other projects can use it
get_property(PIPELIB_INCLUDE_DIRS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
set(PIPELIB_INCLUDE_DIRS
  ${PIPELIB_INCLUDE_DIRS}
  CACHE FILEPATH INTERNAL FORCE
)


###########
## Tests ##
###########

if(PIPELIB_ENABLE_TESTING)
  add_subdirectory(tests)
endif(PIPELIB_ENABLE_TESTING)
