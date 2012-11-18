
#include "RandomStringBlock.h"

#include "IPipeline.h"
#include "PipeBlock.h"

using namespace pipelib;

RandomStringBlock::RandomStringBlock(const size_t numStrings):
Block< ::std::string>("Random string block"),
myNumStrings(numStrings)
{}

void RandomStringBlock::start()
{
	for(size_t i = 0; i < myNumStrings; ++i)
	{
		std::string & str = myPipeline->newData();
		str = "random";
		myOutput->in(str);
	}
}
