/*
 * AbstractLinkPipeline.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef ABSTRACT_LINK_PIPELINE_H
#define ABSTRACT_LINK_PIPELINE_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/AbstractPipeline.h"
#include "pipelib/Block.h"
#include "pipelib/ILink.h"
#include "pipelib/StartBlock.h"

#include <set>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace pipelib
{

struct LinkPipelineMetadata : public DataMetadata
{
	size_t position;
};

struct LinkPipelineBlockMetadata : public BlockMetadata
{
	LinkPipelineBlockMetadata(
		const int _position = -1,
		const size_t _numLinks = 0):
		BlockMetadata(_position),
		numLinks(_numLinks)
		{}

	size_t numLinks;
};

template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class AbstractLinkPipeline :
	public AbstractPipeline<PipelineData, SharedData, GlobalData, LinkPipelineMetadata, LinkPipelineBlockMetadata>
{
public:

  AbstractLinkPipeline() {}
	virtual ~AbstractLinkPipeline();

	// From IPipeline /////////////////////////

	virtual void connect(Block<PipelineData, SharedData, GlobalData> & output, PipeBlock<PipelineData, SharedData, GlobalData> & input, const int ChannelTyp = CHANNEL_DEFAULT);

	virtual bool disconnect(Block<PipelineData, SharedData, GlobalData> & output, const ChannelTyp outChannel = CHANNEL_DEFAULT);

	virtual bool hasBlock(Block<PipelineData, SharedData, GlobalData> & block) const;

	// End IPipeline ///////////////////////////

	/** When a link has data pass through it this callback is called. */
	void linkCallback(const ILink<PipelineData, SharedData, GlobalData> & link, PipelineData & data);

protected:

  typedef AbstractPipeline<PipelineData, SharedData, GlobalData, LinkPipelineMetadata, LinkPipelineBlockMetadata> Parent;

  AbstractLinkPipeline(GlobalData * const globalData);

	virtual ILink<PipelineData, SharedData, GlobalData> * createLink() = 0;

	virtual void destroyLink(ILink<PipelineData, SharedData, GlobalData> * const link) = 0;

	ILink<PipelineData, SharedData, GlobalData> * findLinkBetween(
		const Block<PipelineData, SharedData, GlobalData> * const outputter,
		const Block<PipelineData, SharedData, GlobalData> * const inputtee,
		const ChannelTyp channel = CHANNEL_ANY) const;

	ILink<PipelineData, SharedData, GlobalData> * findLinkByChannel(
		const Block<PipelineData, SharedData, GlobalData> * const outputter,
		const ChannelTyp channel = CHANNEL_DEFAULT) const;

	/** A set of all the links that join blocks. */
	::std::set<ILink<PipelineData, SharedData, GlobalData> *>	myLinks;

private:

	typedef AbstractPipeline<
		PipelineData,
    SharedData,
		GlobalData,
		LinkPipelineMetadata,
		LinkPipelineBlockMetadata> MyAbsPipeTyp;

	void generatePosition(Block<PipelineData, SharedData, GlobalData> & block, const size_t pos);
};

// IMPLEMENTATION //////////////////////////////////////////

// AbstractLinkPipeline implementation ////

template <typename PipelineData, typename SharedData, typename GlobalData>
AbstractLinkPipeline<PipelineData, SharedData, GlobalData>::~AbstractLinkPipeline()
{
	using namespace std;

	for(typename set<ILink<PipelineData, SharedData, GlobalData> *>::iterator it = myLinks.begin(),
            end = myLinks.end(); it != end; ++it)
	{
		delete *it;
	}
	myLinks.clear();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void AbstractLinkPipeline<PipelineData, SharedData, GlobalData>::connect(
	Block<PipelineData, SharedData, GlobalData> & outputter,
	PipeBlock<PipelineData, SharedData, GlobalData> & inputtee,
	const ChannelTyp outChannel)
{
	using namespace std;

	PASSERT(outChannel != CHANNEL_ANY);

	if(outChannel == CHANNEL_ALL)
	{
		// Need to connect on all output channels
		for(ChannelTyp i = 0; i < outputter.getNumOutputs(); ++i)
		{
			connect(outputter, inputtee, i);
		}
		return;
	}

	// Check if these two are connected already
	if(!findLinkBetween(&outputter, &inputtee, outChannel))
	{
		// Create the link
		ILink<PipelineData, SharedData, GlobalData> * const link = createLink();

		// Insert the blocks into the pipeline and up the number of links
		++(insertBlock(outputter, LinkPipelineBlockMetadata(-1, 0)).first->second.numLinks);
		++(insertBlock(*link, LinkPipelineBlockMetadata(-1, 0)).first->second.numLinks);
		++(insertBlock(inputtee, LinkPipelineBlockMetadata(-1, 0)).first->second.numLinks);

		// Link them all together
		link->link(outputter, inputtee, outChannel);
	}
}

template <typename PipelineData, typename SharedData, typename GlobalData>
bool AbstractLinkPipeline<PipelineData, SharedData, GlobalData>::disconnect(Block<PipelineData, SharedData, GlobalData> & outputter, const ChannelTyp outChannel)
{
	PASSERT(outChannel != CHANNEL_ANY);

	if(outChannel == CHANNEL_ALL)
	{
		bool success = true;
		for(ChannelTyp i = 0; i < outputter.getNumOutputs(); ++i)
		{
			success &= disconnect(outputter, i);
		}
		return success;
	}

	ILink<PipelineData, SharedData, GlobalData> * link = findLinkByChannel(&outputter, outChannel);

	if(link != NULL)
	{
		typename MyAbsPipeTyp::BlocksMap::iterator it;

		// Output
		it = MyAbsPipeTyp::myBlocks.find(link->getOutput());
		if(it == MyAbsPipeTyp::myBlocks.end())
		{
			throw "Link output not found";
		}
		if(--it->second.numLinks <= 0)
		{
			removeBlock(*it->first);
		}

		// Input
		it = MyAbsPipeTyp::myBlocks.find(&link->getInput());
		if(it == MyAbsPipeTyp::myBlocks.end())
		{
			throw "Link input not found";
		}
		if(--it->second.numLinks <= 0 )
		{
			removeBlock(*it->first);
		}

		// Remove the link
		removeBlock(*link);

		link->unlink();
		destroyLink(link);
		link = NULL;

		return true;
	}
	else
	{
		return false;
	}
}

template <typename PipelineData, typename SharedData, typename GlobalData>
bool AbstractLinkPipeline<PipelineData, SharedData, GlobalData>::hasBlock(Block<PipelineData, SharedData, GlobalData> & block) const
{
	return MyAbsPipeTyp::myBlocks.count(&block) > 0;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
AbstractLinkPipeline<PipelineData, SharedData, GlobalData>::AbstractLinkPipeline(GlobalData * const globalData):
AbstractPipeline<PipelineData, SharedData, GlobalData, LinkPipelineMetadata, LinkPipelineBlockMetadata>(globalData)
{}

template <typename PipelineData, typename SharedData, typename GlobalData>
ILink<PipelineData, SharedData, GlobalData> *
AbstractLinkPipeline<PipelineData, SharedData, GlobalData>::findLinkBetween(
	const Block<PipelineData, SharedData, GlobalData> * const outputter,
	const Block<PipelineData, SharedData, GlobalData> * const inputtee,
	const ChannelTyp channel) const
{
	using std::set;

	PASSERT(!(outputter == NULL && inputtee == NULL));
	PASSERT(channel != CHANNEL_ALL);

	bool found = false;
	ILink<PipelineData, SharedData, GlobalData> * link = NULL;
	for(typename set<ILink<PipelineData, SharedData, GlobalData> * >::const_iterator it =
		myLinks.begin(), end = myLinks.end(); it != end; ++it)
	{
		link = *it;
		bool matchedInOut = false;
		if(outputter == NULL)
		{
			matchedInOut = link->getOutput() == inputtee;
		}
		else if(inputtee == NULL)
		{
			matchedInOut = &link->getInput() == outputter;
		}
		else
		{
			matchedInOut = (&link->getInput() == outputter && link->getOutput() == inputtee);
		}

		if(matchedInOut)
		{
			// Check if the channel criterium is also met
			if(channel == CHANNEL_ANY)
			{
				found = true;
			}
			else
			{
				if(channel == link->getLinkChannel())
				{
					found = true;
				}
			}
		}

		if(found) break;
	}

	return found == true ? link : NULL;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
ILink<PipelineData, SharedData, GlobalData> *
AbstractLinkPipeline<PipelineData, SharedData, GlobalData>::findLinkByChannel(
	const Block<PipelineData, SharedData, GlobalData> * const outputter,
	const ChannelTyp channel) const
{
	using namespace std;

	PASSERT(outputter != NULL);

	bool found = false;
	ILink<PipelineData, SharedData, GlobalData> * link = NULL;
	for(typename set<ILink<PipelineData, SharedData, GlobalData> *>::const_iterator it =
		myLinks.begin(), end = myLinks.end(); it != end; ++it)
	{
		ILink<PipelineData, SharedData, GlobalData> * const ln = *it;
		if(ln->getNumOutputs() > channel && ln->getOutput(channel))
		{
			link = ln;
			break;
		}
	}

	return link;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void AbstractLinkPipeline<PipelineData, SharedData, GlobalData>::linkCallback(
	const ILink<PipelineData, SharedData, GlobalData> & link,
	PipelineData & data)
{
	// TODO: Send message that data is passing through

	const typename MyAbsPipeTyp::BlocksMap::const_iterator it = MyAbsPipeTyp::myBlocks.find(link.getOutput());
	if(it != MyAbsPipeTyp::myBlocks.end())
	{
		// Get the metadata
		const typename MyAbsPipeTyp::DataMap::iterator dataIt(MyAbsPipeTyp::myData.find(&data));
		if(dataIt != MyAbsPipeTyp::myData.end())
		{
			// Save the position of the data for when it moved on to the next block
			dataIt->second.position = it->second.position;
		}
	}
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void AbstractLinkPipeline<PipelineData, SharedData, GlobalData>::generatePosition(
	Block<PipelineData, SharedData, GlobalData> & block, const size_t pos)
{
	const typename MyAbsPipeTyp::BlocksMap::iterator it = MyAbsPipeTyp::myBlocks.find(&block);

	if(it != MyAbsPipeTyp::myBlocks.end())
	{
		// If the position is currently -1 then it hasn't been set yet
		if(it->second.position == -1)
		{
			it->second.position = pos;
			for(size_t i = 0; i < block.getNumOutputs(); ++i)
			{
				Block<PipelineData, SharedData, GlobalData> * const link = block.getOutput(i);
				if(link)
				{
					for(size_t j = 0; j < link->getNumOutputs(); ++j)
					{
						Block<PipelineData, SharedData, GlobalData> * const out = link->getOutput(j);
						generatePosition(*out, pos + 1);
					}
				}
			}
		}
	}

}

}

#endif /* ABSTRACT_LINK_PIPELINE_H */
