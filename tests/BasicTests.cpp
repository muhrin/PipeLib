/*
 * ManagedPipelineTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "pipelibtest.h"

#include <pipelib/pipelib.h>

#include <strings/PrintStringBlock.h>
#include <strings/RandomStringBlock.h>

BOOST_AUTO_TEST_CASE(BasicTest)
{
  typedef pipelib::Pipe< ::std::string, const void *, const void *> Pipe;
  typedef pipelib::SingleThreadedEngine< ::std::string, const void *, const void *> Runner;

  // SETTINGS //////////////
  
  Pipe pipe;

  pipe.addBlock(new PrintStringBlock(2));
  Pipe::StartBlockType * const startBlock = pipe.addBlock(new RandomStringBlock(10))->asStartBlock();
  pipe.setStartBlock(startBlock);

  Runner runner;
  runner.run(pipe);
}
