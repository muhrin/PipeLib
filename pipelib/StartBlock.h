/*
 * StartBlock.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef START_BLOCK_H
#define START_BLOCK_H

// INCLUDES /////////////////////////////////////////////
#include "Block.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib {

template <class DataType, class GlobalDataType = EmptyGlobalData>
class StartBlock : public virtual Block<DataType, GlobalDataType>
{
public:
	StartBlock() : Block("Start block") {}

	virtual void start() = 0;
};

}

#endif /* START_BLOCK_H */
