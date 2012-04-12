/*
 * PrintStringBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef RANDOM_STRING_BLOCK_H
#define RANDOM_STRING_BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include <AbstractSimpleStartBlock.h>

#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////

class RandomStringBlock : public ::pipelib::AbstractSimpleStartBlock<std::string>
{
public:
	RandomStringBlock(const size_t numStrings);

	virtual void start();

private:
	const size_t	myNumStrings;

};

#endif /* RANDOM_STRING_BLOCK_H */
