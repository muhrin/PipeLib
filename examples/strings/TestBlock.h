/*
 * TestBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef TEST_BLOCK_H
#define TEST_BLOCK_H

// INCLUDES /////////////////////////////////////////////

#include <AbstractSimpleBlock.h>
#include <SingleThreadedPipeline.h>

#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////

class TestBlock : public ::pipelib::AbstractSimpleBlock<::std::string>
{
public:

	TestBlock(): Block("Test block") {}

	virtual void in(::std::string & data);
};

#endif /* TEST_BLOCK_H */
