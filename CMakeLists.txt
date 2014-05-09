
cmake_minimum_required(VERSION 2.6)

project(Pipelib)

set(PIPELIB_VERSION_MAJOR 0)
set(PIPELIB_VERSION_MINOR 5)
set(PIPELIB_VERSION_PATCH 3)
set(PIPELIB_VERSION "${PIPELIB_VERSION_MAJOR}.${PIPELIB_VERSION_MINOR}.${PIPELIB_VERSION_PATCH}")
message(STATUS "Configuring Pipelib ${PIPELIB_VERSION}")

## CONFIGURATION SETTINGS ##############################

set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR share/Pipelib/cmake)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# Build options ###
set(PIPELIB_BUILD_TESTS FALSE CACHE BOOL "Build pipelib tests")

configure_file(
  "${PROJECT_SOURCE_DIR}/include/pipelib/PipelibConfig.h.in"
  "${PROJECT_BINARY_DIR}/include/pipelib/PipelibConfig.h"
)

## pipelib

set(pipelib_Header_Files__
#  ${PROJECT_BINARY_DIR}/include/pipelib/PipelibConfig.h
  include/pipelib/Barrier.h
  include/pipelib/Block.h
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


set(PIPELIB_INCLUDE_DIRS
  ${Boost_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/include # So that we find PipelibConfig.h
  ${PROJECT_SOURCE_DIR}/include
  CACHE FILEPATH INTERNAL FORCE
)


## CONFIG - set up export files for projects to use pipelib

# Make relative paths absolute
foreach(p INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# Find the relative path from the cmake to the include folder
file(RELATIVE_PATH PIPELIB_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}"
)
configure_file(cmake/PipelibConfig.cmake.in 
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PipelibConfig.cmake" @ONLY
)
configure_file(cmake/PipelibConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PipelibConfigVersion.cmake" @ONLY
)


## INSTALL
install(DIRECTORY 
  include/pipelib
  DESTINATION "${INSTALL_INCLUDE_DIR}"
  COMPONENT dev
)
install(DIRECTORY
  "${PROJECT_BINARY_DIR}/include/pipelib"
  DESTINATION "${INSTALL_INCLUDE_DIR}"
  COMPONENT dev
)
# TODO: Remove all *.in files that were installed
install(FILES
  ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PipelibConfig.cmake
  ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PipelibConfigVersion.cmake
  DESTINATION "${INSTALL_CMAKE_DIR}"
  COMPONENT dev
)

###########
## Tests ##
###########
if(PIPELIB_BUILD_TESTS)
  add_subdirectory(tests)
endif(PIPELIB_BUILD_TESTS)



