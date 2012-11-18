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
#include "AbstractSimpleBlock.h"

#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////

class PrintStringBlock : public ::pipelib::AbstractSimpleBlock<std::string>
{
public:
	PrintStringBlock(const int & num);


	virtual void in(std::string & data);


private:
	const int myNum;
};

#endif /* PRINT_STRING_BLOCK_H */
