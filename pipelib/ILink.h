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
#include "pipelib/Pipeline.h"
#include "pipelib/PipeBlock.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {
	template <typename PipelineData, typename SharedData, typename GlobalData>
	class Block;
}

namespace pipelib {

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class ILink : public virtual PipeBlock<PipelineData, SharedData, GlobalData>
{
public:

	ILink() : Block<PipelineData, SharedData, GlobalData>("ILink") {}

	virtual void link(
		Block<PipelineData, SharedData, GlobalData> & input,
		PipeBlock<PipelineData, SharedData, GlobalData> & output,
		const ChannelTyp outChannel = CHANNEL_DEFAULT) = 0;

	virtual void unlink() = 0;

	virtual Block<PipelineData, SharedData, GlobalData> & getInput() const = 0;

	virtual ChannelTyp getLinkChannel() const = 0;
};

}

#endif /* I_LINK_H */
