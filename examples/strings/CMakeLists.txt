cmake_minimum_required(VERSION 2.6)
project(Strings)

# Add the pipelib header directory
include_directories(../../pipelib)

# Set all the input files for this examples
set(strings_SRCS
 PrintStringBlock.cpp
 PrintStringBlock.h
 RandomStringBlock.cpp
 RandomStringBlock.h
 strings.cpp
)


add_executable(Strings ${strings_SRCS})
