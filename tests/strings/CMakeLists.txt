
project(strings)

# Add the pipelib header directory
include_directories(${PIPELIB_INCLUDE_DIRS})

# Set all the input files for this examples
set(strings_SRCS
 PrintStringBlock.cpp
 PrintStringBlock.h
 RandomStringBlock.cpp
 RandomStringBlock.h
 strings.cpp
)


add_executable(strings_bin ${strings_SRCS})
add_library(strings ${strings_SRCS})

add_dependencies(strings_bin pipelib)
target_link_libraries(strings_bin pipelib)

