// PipelineLibTest.cpp : Defines the entry point for the console application.
//

#include <pipelib/pipelib.h>

#include "PrintStringBlock.h"
#include "RandomStringBlock.h"

int main()
{
	using namespace pipelib;
	using std::string;

	// Create the pipeline
  NoSharedGlobal<string>::SingleThreadedEngineType engine;

	// Create the start block
	RandomStringBlock start(10);

	// Create pipeline blocks
	PrintStringBlock b1(1);
	PrintStringBlock b2(2);
  pipelib::SimpleBarrier<string, const void *, const void *> barrier;
	PrintStringBlock b3(3);

  // Connect everything
  start |= b1 |= b2 |= barrier |= b3;

	// Run the pipe
  engine.run(start);

	return 0;
}


