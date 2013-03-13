// PipelineLibTest.cpp : Defines the entry point for the console application.
//

#include <pipelib/pipelib.h>

#include "PrintStringBlock.h"
#include "RandomStringBlock.h"

int main()
{
	using namespace pipelib;
	using std::string;

  typedef ::pipelib::Pipe<string, const void *, const void *> PipeType;
  typedef pipelib::SimpleBarrier<string, const void *, const void *> Barrier;
  typedef PipeType::BlockType BlockType;

	// Create the pipeline
  NoSharedGlobal<string>::SingleThreadedEngineType engine;

	// Create the start block
  PipeType pipe;
  RandomStringBlock * const start = pipe.addBlock(new RandomStringBlock(10));
  pipe.setStartBlock(start);

	// Create pipeline blocks
	PrintStringBlock * const b1 = pipe.addBlock(new PrintStringBlock(1));
  PrintStringBlock * const b2 = pipe.addBlock(new PrintStringBlock(2));
  Barrier * const barrier = pipe.addBlock(new Barrier());
	PrintStringBlock * const b3 = pipe.addBlock(new PrintStringBlock(3));

  // Connect everything
  pipe.connect(start, b1);
  pipe.connect(b1, b2);
  pipe.connect(b2, barrier);
  pipe.connect(barrier, b3);

	// Run the pipe
  engine.run(pipe);

	return 0;
}


