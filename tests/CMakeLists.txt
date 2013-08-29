
cmake_minimum_required(VERSION 2.6)

message(STATUS "Configuring Pipelib tests")

find_package(Boost 1.36.0 REQUIRED COMPONENTS unit_test_framework)

add_subdirectory(
 strings
)

## tests/

set(tests_Header_Files__
  pipelibtest.h
)
source_group("Header Files" FILES ${tests_Header_Files__})

set(tests_Source_Files__
  BasicTests.cpp
)
source_group("Source Files" FILES ${tests_Source_Files__})

set(tests_Header_Files
  ${tests_Header_Files__}
)

set(tests_Source_Files
  ${tests_Source_Files__}
)

set(tests_Files
  ${tests_Header_Files}
  ${tests_Source_Files}
)

#########################
## Include directories ##
#########################

include_directories(
  ${PIPELIB_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/tests
)

############################
## PipelibTest executable ##
############################
add_executable(pipelibtest
  ${tests_Files}
  pipelibtest.cpp
)

add_dependencies(pipelibtest spipe)

# Libraries we need to link to
target_link_libraries(pipelibtest
  ${Boost_LIBRARIES}
  strings
)