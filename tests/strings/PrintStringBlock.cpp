
#include <pipelib/pipelib.h>

#include "PrintStringBlock.h"

#ifdef PIPELIB_USE_BOOST_THREAD
#include <boost/thread/thread.hpp>
#endif

#include <iostream>
#include <sstream>

using namespace pipelib;

PrintStringBlock::PrintStringBlock(const int & num) :
    BlockType("Print string"), myNum(num)
{
}

void
PrintStringBlock::in(std::string * const data)
{
#ifdef PIPELIB_USE_BOOST_THREAD
  std::cout << "Hello from thread: " << boost::this_thread::get_id() << "\n";
#endif

  std::cout << myNum << " " << *data << std::endl;

  ::std::stringstream ss;
  ss << *data << " " << myNum;
  *data = ss.str();

  out(data);
}
