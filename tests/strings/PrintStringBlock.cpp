
#include <pipelib/Pipeline.h>

#include "PrintStringBlock.h"

#include <iostream>

using namespace pipelib;

PrintStringBlock::PrintStringBlock(const int & num):
BlockType("Print string"),
myNum(num)
{}

void PrintStringBlock::in(std::string & data)
{
	std::cout << myNum << " " << data << std::endl;

	data += " " + myNum;

  out(data);
}
