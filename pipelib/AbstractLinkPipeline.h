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
#include "AbstractPipeline.h"
#include "ILink.h"
#include "StartBlock.h"

#include <set>

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {
	template <class DataType, class GlobalDataType>
	class Block;
}

namespace pipelib {

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

template <class DataType, class GlobalDataType = DefaultGlobalDataTyp>
class AbstractLinkPipeline :
	public AbstractPipeline<DataType, GlobalDataType, LinkPipelineMetadata, LinkPipelineBlockMetadata>
{
public:
	virtual ~AbstractLinkPipeline();

	// From IPipeline /////////////////////////

	virtual void connect(Block<DataType, GlobalDataType> & output, PipeBlock<DataType, GlobalDataType> & input, const int ChannelTyp = CHANNEL_DEFAULT);

	virtual bool disconnect(Block<DataType, GlobalDataType> & output, const ChannelTyp outChannel = CHANNEL_DEFAULT);

	virtual bool hasBlock(Block<DataType, GlobalDataType> & block) const;

	// End IPipeline ///////////////////////////

	/** When a link has data pass through it this callback is called. */
	void linkCallback(const ILink<DataType, GlobalDataType> & link, DataType & data);

protected:


	virtual ILink<DataType, GlobalDataType> * createLink() = 0;

	virtual void destroyLink(ILink<DataType, GlobalDataType> * const link) = 0;

	ILink<DataType, GlobalDataType> * findLinkBetween(
		const Block<DataType, GlobalDataType> * const outputter,
		const Block<DataType, GlobalDataType> * const inputtee,
		const ChannelTyp channel = CHANNEL_ANY) const;

	ILink<DataType, GlobalDataType> * findLinkByChannel(
		const Block<DataType, GlobalDataType> * const outputter,
		const ChannelTyp channel = CHANNEL_DEFAULT) const;

	/** A set of all the links that join blocks. */
	::std::set<ILink<DataType, GlobalDataType> *>	myLinks;

private:

	typedef AbstractPipeline<
		DataType,
		GlobalDataType,
		LinkPipelineMetadata,
		LinkPipelineBlockMetadata> MyAbsPipeTyp;

	void generatePosition(Block<DataType, GlobalDataType> & block, const size_t pos);
};

// IMPLEMENTATION //////////////////////////////////////////

// AbstractLinkPipeline implementation ////

template <class DataType, class GlobalDataType>
AbstractLinkPipeline<DataType, GlobalDataType>::~AbstractLinkPipeline()
{
	using namespace std;

	for(typename set<ILink<DataType, GlobalDataType> *>::iterator it = myLinks.begin(),
            end = myLinks.end(); it != end; ++it)
	{
		delete *it;
	}
	myLinks.clear();
}

template <class DataType, class GlobalDataType>
void AbstractLinkPipeline<DataType, GlobalDataType>::connect(
	Block<DataType, GlobalDataType> & outputter,
	PipeBlock<DataType, GlobalDataType> & inputtee,
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
		ILink<DataType, GlobalDataType> * const link = createLink();

		// Insert the blocks into the pipeline and up the number of links
		++(insertBlock(outputter, LinkPipelineBlockMetadata(-1, 0)).first->second.numLinks);
		++(insertBlock(*link, LinkPipelineBlockMetadata(-1, 0)).first->second.numLinks);
		++(insertBlock(inputtee, LinkPipelineBlockMetadata(-1, 0)).first->second.numLinks);

		// Link them all together
		link->link(outputter, inputtee, outChannel);
	}
}

template <class DataType, class GlobalDataType>
bool AbstractLinkPipeline<DataType, GlobalDataType>::disconnect(Block<DataType, GlobalDataType> & outputter, const ChannelTyp outChannel)
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

	ILink<DataType, GlobalDataType> * link = findLinkByChannel(&outputter, outChannel);

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

template <class DataType, class GlobalDataType>
bool AbstractLinkPipeline<DataType, GlobalDataType>::hasBlock(Block<DataType, GlobalDataType> & block) const
{
	return MyAbsPipeTyp::myBlocks.count(&block) > 0;
}

template <class DataType, class GlobalDataType>
ILink<DataType, GlobalDataType> *
AbstractLinkPipeline<DataType, GlobalDataType>::findLinkBetween(
	const Block<DataType, GlobalDataType> * const outputter,
	const Block<DataType, GlobalDataType> * const inputtee,
	const ChannelTyp channel) const
{
	using std::set;

	PASSERT(!(outputter == NULL && inputtee == NULL));
	PASSERT(channel != CHANNEL_ALL);

	bool found = false;
	ILink<DataType, GlobalDataType> * link = NULL;
	for(typename set<ILink<DataType, GlobalDataType> * >::const_iterator it =
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

template <class DataType, class GlobalDataType>
ILink<DataType, GlobalDataType> *
AbstractLinkPipeline<DataType, GlobalDataType>::findLinkByChannel(
	const Block<DataType, GlobalDataType> * const outputter,
	const ChannelTyp channel) const
{
	using namespace std;

	PASSERT(outputter != NULL);

	bool found = false;
	ILink<DataType, GlobalDataType> * link = NULL;
	for(typename set<ILink<DataType, GlobalDataType> *>::const_iterator it =
		myLinks.begin(), end = myLinks.end(); it != end; ++it)
	{
		ILink<DataType, GlobalDataType> * const ln = *it;
		if(ln->getNumOutputs() > channel && ln->getOutput(channel))
		{
			link = ln;
			break;
		}
	}

	return link;
}

template <class DataType, class GlobalDataType>
void AbstractLinkPipeline<DataType, GlobalDataType>::linkCallback(
	const ILink<DataType, GlobalDataType> & link,
	DataType & data)
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

template <class DataType, class GlobalDataType>
void AbstractLinkPipeline<DataType, GlobalDataType>::generatePosition(
	Block<DataType, GlobalDataType> & block, const size_t pos)
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
				Block<DataType, GlobalDataType> * const link = block.getOutput(i);
				if(link)
				{
					for(size_t j = 0; j < link->getNumOutputs(); ++j)
					{
						Block<DataType, GlobalDataType> * const out = link->getOutput(j);
						generatePosition(*out, pos + 1);
					}
				}
			}
		}
	}

}

}

#endif /* ABSTRACT_LINK_PIPELINE_H */
