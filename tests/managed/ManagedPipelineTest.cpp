/*
 * ManagedPipelineTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "pipelibtest.h"

#include <pipelib/SingleThreadedEngine.h>
#include <pipelib/managed/ManagedPipeline.h>
#include <pipelib/managed/ManagedBlock.h>

#include <strings/PrintStringBlock.h>
#include <strings/RandomStringBlock.h>

typedef ::pipelib::managed::ManagedBlock< ::std::string, const void *, const void *> ManagedStringBlock;

ManagedStringBlock createPipe()
{
  ManagedStringBlock randStrings = ManagedStringBlock::create(::pipelib::makeUniquePtr(new RandomStringBlock(10)));
  ManagedStringBlock printStrings = ManagedStringBlock::create(::pipelib::makeUniquePtr(new PrintStringBlock(10)));
  randStrings |= printStrings |= ManagedStringBlock::create(::pipelib::makeUniquePtr(new PrintStringBlock(10))); 
  return randStrings;
}

BOOST_AUTO_TEST_CASE(ManagedPipelineTest)
{
  typedef ::pipelib::managed::ManagedPipeline< ::std::string, const void *, const void *> ManagedPipe;
  // SETTINGS //////////////

  ManagedPipe managedPipe;

  ManagedPipe::ManagedBlockHandle randStrings(managedPipe.insert(::pipelib::UniquePtr<RandomStringBlock>::Type(new RandomStringBlock(10))));
  BOOST_REQUIRE(randStrings.valid());
  ManagedPipe::ManagedBlockHandle printStrings(managedPipe.insert(new PrintStringBlock(10)));
  BOOST_REQUIRE(printStrings.valid());
  managedPipe.insert(new RandomStringBlock(10));

  BOOST_REQUIRE(managedPipe.connect(randStrings, printStrings) == ::pipelib::managed::PipeManipulationOutcome::SUCCESS);
}

BOOST_AUTO_TEST_CASE(ManagedBlockTest)
{
  // SETTINGS //////////////

  ManagedStringBlock pipe = createPipe();

  ::pipelib::SingleThreadedEngine< ::std::string, const void *, const void *> engine;

  engine.runProxy(pipe);
}
