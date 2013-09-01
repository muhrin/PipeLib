#include <pipelib/Pipeline.h>

#include "PrintStringBlock.h"

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
  std::cout << myNum << " " << *data << std::endl;

  ::std::stringstream ss;
  ss << *data << " " << myNum;
  *data = ss.str();

  out(data);
}
