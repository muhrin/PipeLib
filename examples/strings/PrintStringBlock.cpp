
#include "Pipeline.h"

#include "PrintStringBlock.h"

#include <iostream>

using namespace pipelib;

PrintStringBlock::PrintStringBlock(const int & num):
Block("PrintStringBlock"),
myNum(num)
{}

void PrintStringBlock::in(std::string & data)
{
	std::cout << myNum << " " << data << std::endl;

	data += " " + myNum;

	myOutput->in(data);
}