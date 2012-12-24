/*
 * PrintStringBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PRINT_STRING_BLOCK_H
#define PRINT_STRING_BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include <string>

#include <pipelib/pipelib.h>

// FORWARD DECLARATIONS ////////////////////////////////////

// TYPEDEFS //////////////////////////////////////////////

typedef ::pipelib::NoSharedGlobal< ::std::string>::PipeBlockType StringPipeBlock;

class PrintStringBlock : public StringPipeBlock
{
  typedef StringPipeBlock Base;
  typedef Base::BlockType BlockType;
public:
	PrintStringBlock(const int & num);

  virtual void in(::std::string & data);

private:
	const int myNum;
};

#endif /* PRINT_STRING_BLOCK_H */
