// PipelineLibTest.cpp : Defines the entry point for the console application.
//

#include <pipelib/pipelib.h>

#include "PrintStringBlock.h"
#include "RandomStringBlock.h"

int
main()
{
  using namespace pipelib;
  using std::string;

  typedef pipelib::Block< string, const void *, const void *> BlockType;
  typedef pipelib::BlockHandle< string, const void *, const void *>::Type BlockHandle;
  typedef pipelib::SimpleBarrier< string, const void *, const void *> Barrier;

  // Create the pipeline
  NoSharedGlobal< string>::SerialEngine engine;

  // Create the start block
  BlockHandle start(new RandomStringBlock(3));

  // Create pipeline blocks
  BlockHandle b1(new PrintStringBlock(1));
  BlockHandle b2(new PrintStringBlock(2));
  BlockHandle barrier(new Barrier());
  BlockHandle b3(new PrintStringBlock(3));

  // Connect everything
  start->connect(b1)->connect(b2)->connect(barrier)->connect(b3);

  // Run the pipe
  engine.run(start);

  return 0;
}

