/*
 * PipeBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_BLOCK_H
#define PIPE_BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include "Block.h"

#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
class PipeBlock : public virtual Block<DataType, GlobalDataType>
{
public:

	PipeBlock(): Block("Pipe block") {}

	virtual void in(DataType & data) = 0;
};

}

#endif /* PIPE_BLOCK_H */
