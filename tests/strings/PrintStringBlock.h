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
  typedef pipelib::Block< ::std::string, const void *, const void *> BlockType;
  typedef StringPipeBlock Base;
public:
  PrintStringBlock(const int & num);
  virtual ~PrintStringBlock() {}

  virtual void
  in(::std::string * const data);

private:
  const int myNum;
};

#endif /* PRINT_STRING_BLOCK_H */
