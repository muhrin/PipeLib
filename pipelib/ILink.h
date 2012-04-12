/*
 * ILink.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef I_LINK_H
#define I_LINK_H

// INCLUDES /////////////////////////////////////////////
#include "PipeBlock.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {
	template <class DataType, class GlobalDataType = EmptyGlobalData>
	class Block;
}

namespace pipelib {

template <class DataType, class GlobalDataType = EmptyGlobalData>
class ILink : public virtual PipeBlock<DataType, GlobalDataType>
{
public:

	ILink() : Block("ILink") {}

	virtual void link(
		Block<DataType, GlobalDataType> & input,
		PipeBlock<DataType, GlobalDataType> & output,
		const size_t outChannel = CHANNEL_DEFAULT) = 0;

	virtual void unlink() = 0;

	virtual Block<DataType, GlobalDataType> & getInput() const = 0;

	virtual int getLinkChannel() const = 0;
};

}

#endif /* I_LINK_H */
