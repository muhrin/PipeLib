/*
 * AbstractPipeline.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef ABSTRACT_PIPELINE_H
#define ABSTRACT_PIPELINE_H

// INCLUDES /////////////////////////////////////////////
#include "Pipeline.h"
#include "EmptyGlobalData.h"
#include "IDataSink.h"
#include "IFinishNotifiable.h"
#include "IPipeline.h"
#include "SimpleEndBlock.h"
#include "DefaultBarrier.h"

#include <algorithm>
#include <limits>
#include <map>
#include <vector>

// DEFINES ///////////////////////////////////////////////
#define MAX_BARRIERS 10000

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {
	class GlobalDataType;
}

namespace pipelib {

struct DataMetadata
{
	enum State { CURRENT, DROPPED, FINISHED };

	DataMetadata(): referenceCount(1), state(CURRENT) {}
	virtual ~DataMetadata() {}

	size_t	referenceCount;
	State	state;
};

struct BlockMetadata
{
	BlockMetadata(const int _position):
		position(_position)	{}
	virtual ~BlockMetadata() {}

	int		position;
};


template <
	class DataType,
	class GlobalDataType	= DefaultGlobalDataTyp,
	class MetadataType		= DataMetadata,
	class BlockMetadataType	= BlockMetadata>
class AbstractPipeline :
	public IPipeline<DataType, GlobalDataType>,
	public IFinishNotifiable<DataType>
{
public:
	AbstractPipeline();
	virtual ~AbstractPipeline();

	// Blocks ////////////////////
	virtual StartBlock<DataType, GlobalDataType> * getStartBlock() const;
	virtual void setStartBlock(StartBlock<DataType, GlobalDataType> & startBlock);
	virtual bool clearStartBlock();
	virtual bool hasPathBetween(const Block<DataType, GlobalDataType> & from, const Block<DataType, GlobalDataType> & to) const;

	// Pipeline methods ///////////
	virtual bool isConstructed() const;
	virtual bool initialise();
	virtual void start();

	// Barriers /////////////
	virtual Barrier<DataType, GlobalDataType> * createBarrier();
	virtual void registerBarrier(Barrier<DataType, GlobalDataType> & barrier);
	virtual bool deregisterBarrier(Barrier<DataType, GlobalDataType> & barrier);

	// Data //////////////////////
	virtual GlobalDataType & getSharedData();
	virtual DataType & newData();
	virtual void registerNewData(DataType * const data);
	virtual void dropData(DataType & data);
	virtual void flagData(const Block<DataType, GlobalDataType> & block, DataType & data);
	virtual void unflagData(const Block<DataType, GlobalDataType> & block, DataType & data);
	virtual void setFinishedDataSink(IDataSink<DataType> & sink);

	// From IFinishNotifiable
	virtual void dataFinished(DataType & data);

protected:

	// TYPEDEFS //////////////////////////////////////

	/** Data container types */
	typedef typename ::std::map<DataType *, MetadataType> DataMap;
	typedef typename ::std::pair<DataType *, MetadataType> DataMapPair;
	/** Block metadata container types */
	typedef typename ::std::map<Block<DataType, GlobalDataType> *, BlockMetadataType> BlocksMap;
	typedef typename ::std::pair<Block<DataType, GlobalDataType> *, BlockMetadataType> BlocksMapPair;
	/** Barrier container types */
	typedef typename ::std::vector<Barrier<DataType, GlobalDataType> *> BarriersContainer;
	/** End blocks container type */
	typedef typename ::std::vector<SimpleEndBlock<DataType, GlobalDataType> * > EndBlocksContainer;

	// Blocks ///////////////////////////////
	virtual ::std::pair<typename BlocksMap::iterator, bool> insertBlock(
		Block<DataType, GlobalDataType> & block,
		const BlockMetadataType & blockMetadata);
	virtual void removeBlock(Block<DataType, GlobalDataType> & block);

	// Data ////////////////////////////////
	virtual void flagData(const typename DataMap::iterator & it);
	virtual void unflagData(const typename DataMap::iterator & it);

	// Barriers /////////////////////////////
	virtual bool releaseNextBarrier();

	/** The pipeline start block */
	StartBlock<DataType, GlobalDataType> *	myStartBlock;

	GlobalDataType						mySharedData;

	/** A map containing all the blocks and block metadata. */
	BlocksMap							myBlocks;

	DataMap								myData;

	BarriersContainer					myBarriers;

	IDataSink<DataType> *				myFinishedDataSink;

	EndBlocksContainer					myEndBlocks;

private:
	bool	myInitialised;
};

// IMPLEMENTATION ///////////////////////////

// DEFINES //////////////////////////////////
#define ABSTRACT_PIPELINE_TPARAMS	<class DataType, class GlobalDataType, class MetadataType, class BlockMetadataType>
#define ABSTRACT_PIPELINE_TTYPE		AbstractPipeline<DataType, GlobalDataType, MetadataType, BlockMetadataType>

// AbstractPipeline implementation

template ABSTRACT_PIPELINE_TPARAMS
ABSTRACT_PIPELINE_TTYPE::AbstractPipeline():
myStartBlock(NULL),
myFinishedDataSink(NULL),
myInitialised(false)
{}

template ABSTRACT_PIPELINE_TPARAMS
ABSTRACT_PIPELINE_TTYPE::~AbstractPipeline()
{
	// Clean up my end blocks
	for(typename EndBlocksContainer::iterator it = myEndBlocks.begin(), end = myEndBlocks.end();
		it != end; ++it)
	{
		delete *it;
	}

	// Clean up the data
	for(typename DataMap::iterator it = myData.begin(), end = myData.end();
		it != end; ++it)
	{
		delete it->first;
	}
}

template ABSTRACT_PIPELINE_TPARAMS
StartBlock<DataType, GlobalDataType> * ABSTRACT_PIPELINE_TTYPE::getStartBlock() const
{
	return myStartBlock;
}


template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::setStartBlock(StartBlock<DataType, GlobalDataType> & startBlock)
{
	if(isConstructed())
	{
		throw "Cannot set start block after pipeline has been initialised.";
	}

	StartBlock<DataType, GlobalDataType> * const oldStartBlock = myStartBlock;
	insertBlock(startBlock, BlockMetadataType());
	myStartBlock = &startBlock;

	if(oldStartBlock != NULL)
	{
		const size_t oldOutputs = oldStartBlock->getNumOutputs();
		const size_t newOutputs = myStartBlock->getNumOutputs();
		// Try to transfer the connections between the old start block and its outputs
		// to the new start block
		for(ChannelTyp i = 0; i < oldOutputs; ++i)
		{
			PipeBlock<DataType, GlobalDataType> * const output = oldStartBlock->getOutput(i);
			if(output)
			{
				disconnect(*oldStartBlock, i);
				if(i < newOutputs)
				{
					connect(*myStartBlock, *output, i);
				}
			}
		}
	}
}

template ABSTRACT_PIPELINE_TPARAMS
bool ABSTRACT_PIPELINE_TTYPE::clearStartBlock()
{
	if(isConstructed())
	{
		throw "Cannot set start block after pipeline has been initialised.";
	}

	if(!myStartBlock)
		return false;

	myStartBlock->clearOutput(CHANNEL_ALL);
	myStartBlock = NULL;
	return true;
}

template ABSTRACT_PIPELINE_TPARAMS
bool ABSTRACT_PIPELINE_TTYPE::initialise()
{
	if(isConstructed())
	{
		throw "Cannot initialise the pipeline more than once";
	}

	// Go through all the blocks and add an end block to delete the data
	// once the end is reached
	for(typename BlocksMap::iterator it = myBlocks.begin(), end = myBlocks.end();
		it != end; ++it)
	{
		Block<DataType, GlobalDataType> * const block = it->first;

		// Make sure we cover all outputs
		for(ChannelTyp i = 0; i < block->getNumOutputs(); ++i)
		{
			if(!block->getOutput(i))
			{
				SimpleEndBlock<DataType, GlobalDataType> * const end = new SimpleEndBlock<DataType, GlobalDataType>(*this);
				myEndBlocks.push_back(end);
				connect(*block, *end, i);
			}
		}
	}

	// Tell all the blocks that the pipeline is initialising
	for(typename BlocksMap::iterator it = myBlocks.begin(), end = myBlocks.end();
		it != end; ++it)
	{
		it->first->pipelineInitialising();
	}

	myInitialised = true;

	// Tell all the blocks that they have been intialised
	for(typename BlocksMap::iterator it = myBlocks.begin(), end = myBlocks.end();
		it != end; ++it)
	{
		it->first->pipelineInitialised();
	}

	return true;

	return myInitialised;
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::start()
{
	if(!isConstructed())
	{
		throw "Pipeline cannot be started before it is initialised.";
	}

	// Tell all the blocks that we are starting
	for(typename BlocksMap::iterator it = myBlocks.begin(), end = myBlocks.end();
		it != end; ++it)
	{
		it->first->pipelineStarting();
	}

	myStartBlock->start();

	// Keep release all the barriers along the way one by one
	for(size_t i = 0; releaseNextBarrier(); ++i)
	{
		if(i == MAX_BARRIERS)
		{
			throw "Barriers loop exceeded the limit, probably something wrong with the \
				  Barrier implementation";
		}
	}

	// Tell the blocks that we are finishing
	// Tell all the blocks that we are starting
	for(typename BlocksMap::iterator it = myBlocks.begin(), end = myBlocks.end();
		it != end; ++it)
	{
		it->first->pipelineFinishing();
	}
}


template ABSTRACT_PIPELINE_TPARAMS
bool ABSTRACT_PIPELINE_TTYPE::hasPathBetween(
	const Block<DataType, GlobalDataType> & from,
	const Block<DataType, GlobalDataType> & to) const
{
	bool pathFound = false;

	for(ChannelTyp i = 0; i < from.getNumOutputs(); ++i)
	{
		const Block<DataType, GlobalDataType> * const output = from.getOutput(i);
		if(output)
		{
			if(output == &to)
			{
				pathFound = true;
			}
			else
			{
				pathFound = hasPathBetween(*output, to);
			}

			if(pathFound) break;
		}
	}

	return pathFound;
}

template ABSTRACT_PIPELINE_TPARAMS
pipelib::Barrier<DataType, GlobalDataType> * ABSTRACT_PIPELINE_TTYPE::createBarrier()
{
	DefaultBarrier<DataType, GlobalDataType> * const barrier = new DefaultBarrier<DataType, GlobalDataType>();
	return barrier;
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::registerBarrier(
	Barrier<DataType, GlobalDataType> & barrier)
{
	myBarriers.push_back(&barrier);	
}

template ABSTRACT_PIPELINE_TPARAMS
bool ABSTRACT_PIPELINE_TTYPE::deregisterBarrier(
	Barrier<DataType, GlobalDataType> & barrier)
{
	typename BarriersContainer::iterator it =
		::std::find(myBarriers.begin(), myBarriers.end(), &barrier);

	bool found = false;
	if(it != myBarriers.end())
	{
		myBarriers.erase(it);
		found = true;
	}
	return found;
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::setFinishedDataSink(IDataSink<DataType> & sink)
{
	if(isConstructed())
		throw "Cannot set data sink after the pipeline has been constructed";

	myFinishedDataSink = &sink;
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::dataFinished(DataType & data)
{
	typename DataMap::iterator it = myData.find(&data);
	if(it != myData.end())
	{
		it->second.state = DataMetadata::FINISHED;
		unflagData(it);
	}
}

template ABSTRACT_PIPELINE_TPARAMS
GlobalDataType & ABSTRACT_PIPELINE_TTYPE::getSharedData()
{
	return mySharedData;
}

template ABSTRACT_PIPELINE_TPARAMS
bool ABSTRACT_PIPELINE_TTYPE::isConstructed() const
{
	return myInitialised;
}

template ABSTRACT_PIPELINE_TPARAMS
DataType & ABSTRACT_PIPELINE_TTYPE::newData()
{
	// Create data
	DataType * const data = new DataType();
	// Register it
	registerNewData(data);
	return *data;
}

template ABSTRACT_PIPELINE_TPARAMS
void  ABSTRACT_PIPELINE_TTYPE::registerNewData(DataType * const data)
{
	// Create metadata
	myData.insert(DataMapPair(data, MetadataType()));
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::dropData(DataType & data)
{
	typename DataMap::iterator it = myData.find(&data);
	if(it != myData.end())
	{
		it->second.state = DataMetadata::DROPPED;
		unflagData(it);
	}
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::flagData(const Block<DataType, GlobalDataType> & block, DataType & data)
{
	typename DataMap::iterator it = myData.find(&data);
	if(it !=  myData.end())
	{
		flagData(it);
	}
}


template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::unflagData(const Block<DataType, GlobalDataType> & block, DataType & data)
{
	typename DataMap::iterator it = myData.find(&data);
	if(it !=  myData.end())
	{
		unflagData(it);
	}
}

template ABSTRACT_PIPELINE_TPARAMS
::std::pair<typename ABSTRACT_PIPELINE_TTYPE::BlocksMap::iterator, bool>
ABSTRACT_PIPELINE_TTYPE::insertBlock(
		Block<DataType, GlobalDataType> & block,
		const BlockMetadataType & blockMetadata)
{
	::std::pair<typename BlocksMap::iterator, bool> result =
		myBlocks.insert(BlocksMapPair(&block, blockMetadata));
	// Was the block newly inserted?
	if(result.second)
		block.inserted(*this);
	return result;
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::removeBlock(
		Block<DataType, GlobalDataType> & block)
{
	const typename BlocksMap::iterator it = myBlocks.find(&block);
	if(it == myBlocks.end())
	{
		throw "Trying to remove block that is not in the pipeline";
	}
	myBlocks.erase(it);
	block.removed();
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::flagData(const typename DataMap::iterator & it)
{
	PASSERT(it != myData.end());

	// Up the reference count
	++(it->second.referenceCount);
}


template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::unflagData(const typename DataMap::iterator & it)
{
	PASSERT(it != myData.end());

	// TODO: Deal with dropped/finished

	// Reduce the reference count, delete if 0
	if(--(it->second.referenceCount) <= 0)
	{
		// See if we have a sink that wants the data
		if(it->second.state == DataMetadata::FINISHED && myFinishedDataSink)
		{
			myFinishedDataSink->in(it->first);
		}
		else
		{
			// Delete the data instance
			delete it->first;
		}
		// Remove the data/metadata entry
		myData.erase(it);
	}
}

template ABSTRACT_PIPELINE_TPARAMS
bool ABSTRACT_PIPELINE_TTYPE::releaseNextBarrier()
{
	Barrier<DataType, GlobalDataType> * toRelease = NULL;
	int minPosition = ::std::numeric_limits<int>::max();

	// Find the barrier that has buffered data with the lowest position
	for(typename BarriersContainer::iterator it = myBarriers.begin(), end = myBarriers.end();
		it != end; ++it)
	{
		Barrier<DataType, GlobalDataType> * const bar = *it;
		if(bar->hasData())
		{
			typename BlocksMap::iterator itBlocks = myBlocks.find(bar);
			if(itBlocks != myBlocks.end())
			{
				if(itBlocks->second.position < minPosition)
				{
					minPosition = itBlocks->second.position;
					toRelease = bar;
				}
			}
		}
	}

	if(toRelease)
		toRelease->release();

	return toRelease != NULL;
}

}

#endif /* ABSTRACT_PIPELINE_H */
