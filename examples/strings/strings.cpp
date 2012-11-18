// PipelineLibTest.cpp : Defines the entry point for the console application.
//

#include <DirectPipeline.h>
#include <SingleThreadedPipeline.h>

#include "PrintStringBlock.h"
#include "RandomStringBlock.h"


int main()
{
	using namespace pipelib;
	using std::string;

	// Create the pipeline
	SingleThreadedPipeline<string> pipe = SingleThreadedPipeline<string>();

	// Create the start block
	RandomStringBlock start(10);

	// Create pipeline blocks
	PrintStringBlock b1(1);
	PrintStringBlock b2(2);
	PrintStringBlock b3(3);

	pipe.setStartBlock(start);
	pipe.connect(start, b1);
	pipe.connect(b1, b2);
	pipe.connect(b2, b3);

	// Initialise the pipe
	pipe.initialise();

	// Start the pipe
	pipe.start();

	return 0;
}


