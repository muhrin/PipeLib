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
#include <string>

#include <pipelib/pipelib.h>

typedef ::pipelib::NoSharedGlobal< ::std::string>::StartBlockType StringStartBlock;

class RandomStringBlock : public StringStartBlock
{
  typedef StringStartBlock Base;
  typedef Base::BlockType BlockType;
public:
	RandomStringBlock(const size_t numStrings);

	virtual void start();

private:
	const size_t	myNumStrings;

};

#endif /* RANDOM_STRING_BLOCK_H */
