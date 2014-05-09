
cmake_minimum_required(VERSION 2.6)

message(STATUS "Configuring Pipelib tests")

## CONFIGURATION SETTINGS ##############################

# Build options ###
if(NOT Boost_USE_STATIC_LIBS)
  add_definitions(-DBOOST_TEST_DYN_LINK)
endif(NOT Boost_USE_STATIC_LIBS)

## END CONFIGURATION SETTINGS ##########################
find_package(Boost 1.36.0 QUIET REQUIRED COMPONENTS unit_test_framework)

## tests/

set(test_folders
  strings
)

foreach(test_folder ${test_folders})
  # Look for cpp files
  file(GLOB
    tests_Source_Files__${test_folder}
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${test_folder}/*.cpp
  )
  source_group("Source Files\\${test_folder}" FILES ${tests_Source_Files__${test_folder}})
  set(tests_Source_Files
    ${tests_Source_Files}
	${tests_Source_Files__${test_folder}}
  )
  # Look for h files
  file(GLOB
    tests_Header_Files__${test_folder}
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${test_folder}/*.h
  )
  source_group("Header Files\\${test_folder}" FILES ${tests_Header_Files__${test_folder}})
  set(tests_Header_Files
    ${tests_Header_Files}
	${tests_Header_Files__${test_folder}}
  )
  
  # Look for input files
  file(GLOB
    tests_Input_Files__${test_folder}
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${test_folder}/input/*
  )
  set(tests_Input_Files
    ${tests_Input_Files}
	${tests_Input_Files__${test_folder}}
  )
endforeach()

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

add_dependencies(pipelibtest pipelib)

# Libraries we need to link to
target_link_libraries(pipelibtest
  ${Boost_LIBRARIES}
)
