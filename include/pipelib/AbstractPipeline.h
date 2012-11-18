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
#include <algorithm>
#include <limits>
#include <map>
#include <vector>


#include "pipelib/DefaultBarrier.h"
#include "pipelib/EmptyGlobalData.h"
#include "pipelib/IDataSink.h"
#include "pipelib/IFinishNotifiable.h"
#include "pipelib/IPipeline.h"
#include "pipelib/Pipeline.h"
#include "pipelib/PipelineState.h"
#include "pipelib/SimpleEndBlock.h"
#include "pipelib/event/EventSupport.h"
#include "pipelib/event/IPipeListener.h"
#include "pipelib/event/PipeStateChanged.h"



// DEFINES ///////////////////////////////////////////////
#define MAX_BARRIERS 10000

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
	typename PipelineData,
  typename SharedData     = DefaultSharedData,
	typename GlobalData     = SharedData,
	class MetadataType		  = DataMetadata,
	class BlockMetadataType	= BlockMetadata>
class AbstractPipeline :
	public IPipeline<PipelineData, SharedData, GlobalData>,
	public IFinishNotifiable<PipelineData>
{
public:
	AbstractPipeline();
	virtual ~AbstractPipeline();
	
  typedef IPipeline<PipelineData, SharedData, GlobalData> IPipelineTyp;

	// Blocks ////////////////////
	virtual StartBlock<PipelineData, SharedData, GlobalData> * getStartBlock() const;
	virtual void setStartBlock(StartBlock<PipelineData, SharedData, GlobalData> & startBlock);
	virtual bool clearStartBlock();
	virtual bool hasPathBetween(const Block<PipelineData, SharedData, GlobalData> & from, const Block<PipelineData, SharedData, GlobalData> & to) const;

	// Pipeline methods ///////////
  virtual PipelineState::Value getState() const;
	virtual bool isInitialised() const;
	virtual bool initialise();
	virtual void start();

	// Barriers /////////////
	virtual void registerBarrier(Barrier<PipelineData, SharedData, GlobalData> & barrier);
	virtual bool deregisterBarrier(Barrier<PipelineData, SharedData, GlobalData> & barrier);

	// Data //////////////////////
	virtual SharedData & getSharedData();
  virtual const SharedData & getSharedData() const;
  virtual GlobalData & getGlobalData();
  virtual const GlobalData & getGlobalData() const;
	virtual PipelineData & newData();
	virtual void registerNewData(PipelineData * const data);
	virtual void dropData(PipelineData & data);
	virtual void flagData(const Block<PipelineData, SharedData, GlobalData> & block, PipelineData & data);
	virtual void unflagData(const Block<PipelineData, SharedData, GlobalData> & block, PipelineData & data);
	virtual void setFinishedDataSink(IDataSink<PipelineData> & sink);

	// From IFinishNotifiable
	virtual void dataFinished(PipelineData & data);

  // Event ///////////////////////
  virtual void addPipeListener(event::IPipeListener<IPipelineTyp> & listener);
  virtual bool removePipeListener(event::IPipeListener<IPipelineTyp> & listener);

protected:

	// TYPEDEFS //////////////////////////////////////

	/** Data container types */
	typedef typename ::std::map<PipelineData *, MetadataType> DataMap;
	/** Block metadata container types */
	typedef typename ::std::map<Block<PipelineData, SharedData, GlobalData> *, BlockMetadataType> BlocksMap;
	/** Barrier container types */
	typedef typename ::std::vector<Barrier<PipelineData, SharedData, GlobalData> *> BarriersContainer;
	/** End blocks container type */
	typedef typename ::std::vector<SimpleEndBlock<PipelineData, SharedData, GlobalData> * > EndBlocksContainer;

  /**
  /* This constructor is to be used by derived classes when they want to spawn a child.  The children
  /* and the parent must all shared the same global data.
  /**/
  AbstractPipeline(GlobalData * const globalData);

	// Blocks ///////////////////////////////
	virtual ::std::pair<typename BlocksMap::iterator, bool> insertBlock(
		Block<PipelineData, SharedData, GlobalData> & block,
		const BlockMetadataType & blockMetadata);
	virtual void removeBlock(Block<PipelineData, SharedData, GlobalData> & block);

	// Data ////////////////////////////////
	virtual void flagData(const typename DataMap::iterator & it);
	virtual void unflagData(const typename DataMap::iterator & it);

	// Barriers /////////////////////////////
	virtual bool releaseNextBarrier();

  // State ////////////////////////////////
  void changeState(const PipelineState::Value newState);

  PipelineState::Value      myState;

	/** The pipeline start block */
	StartBlock<PipelineData, SharedData, GlobalData> *	myStartBlock;

	SharedData						    mySharedData;
  GlobalData * const        myGlobalData;

	/** A map containing all the blocks and block metadata. */
	BlocksMap							    myBlocks;

	DataMap								    myData;

	BarriersContainer					myBarriers;

	IDataSink<PipelineData> *				myFinishedDataSink;

	EndBlocksContainer					myEndBlocks;

  // Event ///////////////////////////////////
  event::EventSupport<event::IPipeListener<IPipelineTyp> >    myPipeListenerSupport;
};

// IMPLEMENTATION ///////////////////////////

// DEFINES //////////////////////////////////
#define ABSTRACT_PIPELINE_TPARAMS	<typename PipelineData, typename SharedData, typename GlobalData, class MetadataType, class BlockMetadataType>
#define ABSTRACT_PIPELINE_TTYPE		AbstractPipeline<PipelineData, SharedData, GlobalData, MetadataType, BlockMetadataType>

// AbstractPipeline implementation

template ABSTRACT_PIPELINE_TPARAMS
ABSTRACT_PIPELINE_TTYPE::AbstractPipeline():
myState(PipelineState::UNINITIALISED),
myStartBlock(NULL),
myFinishedDataSink(NULL),
myGlobalData(new GlobalData())
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
StartBlock<PipelineData, SharedData, GlobalData> * ABSTRACT_PIPELINE_TTYPE::getStartBlock() const
{
	return myStartBlock;
}


template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::setStartBlock(StartBlock<PipelineData, SharedData, GlobalData> & startBlock)
{
	if(isInitialised())
	{
		throw "Cannot set start block after pipeline has been initialised.";
	}

	StartBlock<PipelineData, SharedData, GlobalData> * const oldStartBlock = myStartBlock;
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
			PipeBlock<PipelineData, SharedData, GlobalData> * const output = oldStartBlock->getOutput(i);
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
	if(isInitialised())
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
	if(isInitialised())
	{
		throw "Cannot initialise the pipeline more than once";
	}

  changeState(PipelineState::INITIALISING);

	// Go through all the blocks and add an end block to delete the data
	// once the end is reached
	for(typename BlocksMap::iterator it = myBlocks.begin(), end = myBlocks.end();
		it != end; ++it)
	{
		Block<PipelineData, SharedData, GlobalData> * const block = it->first;

		// Make sure we cover all outputs
		for(ChannelTyp i = 0; i < block->getNumOutputs(); ++i)
		{
			if(!block->getOutput(i))
			{
				SimpleEndBlock<PipelineData, SharedData, GlobalData> * const end = new SimpleEndBlock<PipelineData, SharedData, GlobalData>(*this);
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


	// Tell all the blocks that they have been intialised
	for(typename BlocksMap::iterator it = myBlocks.begin(), end = myBlocks.end();
		it != end; ++it)
	{
		it->first->pipelineInitialised();
	}

  changeState(PipelineState::INITIALISED);

	return true;
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::start()
{
	if(!isInitialised())
	{
		throw "Pipeline cannot be started before it is initialised.";
	}

  changeState(PipelineState::RUNNING);

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
	for(typename BlocksMap::iterator it = myBlocks.begin(), end = myBlocks.end();
		it != end; ++it)
	{
		it->first->pipelineFinishing();
	}

  changeState(PipelineState::FINISHED);
}


template ABSTRACT_PIPELINE_TPARAMS
bool ABSTRACT_PIPELINE_TTYPE::hasPathBetween(
	const Block<PipelineData, SharedData, GlobalData> & from,
	const Block<PipelineData, SharedData, GlobalData> & to) const
{
	bool pathFound = false;

	for(ChannelTyp i = 0; i < from.getNumOutputs(); ++i)
	{
		const Block<PipelineData, SharedData, GlobalData> * const output = from.getOutput(i);
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
void ABSTRACT_PIPELINE_TTYPE::registerBarrier(
	Barrier<PipelineData, SharedData, GlobalData> & barrier)
{
	myBarriers.push_back(&barrier);	
}

template ABSTRACT_PIPELINE_TPARAMS
bool ABSTRACT_PIPELINE_TTYPE::deregisterBarrier(
	Barrier<PipelineData, SharedData, GlobalData> & barrier)
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
void ABSTRACT_PIPELINE_TTYPE::setFinishedDataSink(IDataSink<PipelineData> & sink)
{
	if(isInitialised())
		throw "Cannot set data sink after the pipeline has been initialised";

	myFinishedDataSink = &sink;
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::dataFinished(PipelineData & data)
{
	typename DataMap::iterator it = myData.find(&data);
	if(it != myData.end())
	{
		it->second.state = DataMetadata::FINISHED;
		unflagData(it);
	}
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::addPipeListener(event::IPipeListener<IPipelineTyp> & listener)
{
  myPipeListenerSupport.insert(listener);
}

template ABSTRACT_PIPELINE_TPARAMS
bool ABSTRACT_PIPELINE_TTYPE::removePipeListener(event::IPipeListener<IPipelineTyp> & listener)
{
  return myPipeListenerSupport.remove(listener);
}

template ABSTRACT_PIPELINE_TPARAMS
SharedData & ABSTRACT_PIPELINE_TTYPE::getSharedData()
{
	return mySharedData;
}

template ABSTRACT_PIPELINE_TPARAMS
const SharedData & ABSTRACT_PIPELINE_TTYPE::getSharedData() const
{
	return mySharedData;
}

template ABSTRACT_PIPELINE_TPARAMS
GlobalData & ABSTRACT_PIPELINE_TTYPE::getGlobalData()
{
	return *myGlobalData;
}

template ABSTRACT_PIPELINE_TPARAMS
const GlobalData & ABSTRACT_PIPELINE_TTYPE::getGlobalData() const
{
	return *myGlobalData;
}

template ABSTRACT_PIPELINE_TPARAMS
PipelineState::Value ABSTRACT_PIPELINE_TTYPE::getState() const
{
  return myState;
}

template ABSTRACT_PIPELINE_TPARAMS
bool ABSTRACT_PIPELINE_TTYPE::isInitialised() const
{
  return myState != PipelineState::UNINITIALISED;
}

template ABSTRACT_PIPELINE_TPARAMS
PipelineData & ABSTRACT_PIPELINE_TTYPE::newData()
{
	// Create data
	PipelineData * const data = new PipelineData();
	// Register it
	registerNewData(data);
	return *data;
}

template ABSTRACT_PIPELINE_TPARAMS
void  ABSTRACT_PIPELINE_TTYPE::registerNewData(PipelineData * const data)
{
	// Create metadata
  myData.insert(typename DataMap::value_type(data, MetadataType()));
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::dropData(PipelineData & data)
{
	typename DataMap::iterator it = myData.find(&data);
	if(it != myData.end())
	{
		it->second.state = DataMetadata::DROPPED;
		unflagData(it);
	}
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::flagData(const Block<PipelineData, SharedData, GlobalData> & block, PipelineData & data)
{
	typename DataMap::iterator it = myData.find(&data);
	if(it !=  myData.end())
	{
		flagData(it);
	}
}


template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::unflagData(const Block<PipelineData, SharedData, GlobalData> & block, PipelineData & data)
{
	typename DataMap::iterator it = myData.find(&data);
	if(it !=  myData.end())
	{
		unflagData(it);
	}
}

template ABSTRACT_PIPELINE_TPARAMS
ABSTRACT_PIPELINE_TTYPE::AbstractPipeline(GlobalData * const globalData):
myState(PipelineState::UNINITIALISED),
myStartBlock(NULL),
myFinishedDataSink(NULL),
myGlobalData(globalData)
{}

template ABSTRACT_PIPELINE_TPARAMS
::std::pair<typename ABSTRACT_PIPELINE_TTYPE::BlocksMap::iterator, bool>
ABSTRACT_PIPELINE_TTYPE::insertBlock(
		Block<PipelineData, SharedData, GlobalData> & block,
		const BlockMetadataType & blockMetadata)
{
	::std::pair<typename BlocksMap::iterator, bool> result =
    myBlocks.insert(typename BlocksMap::value_type(&block, blockMetadata));
	// Was the block newly inserted?
	if(result.second)
		block.inserted(*this);
	return result;
}

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::removeBlock(
		Block<PipelineData, SharedData, GlobalData> & block)
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
	Barrier<PipelineData, SharedData, GlobalData> * toRelease = NULL;
	int minPosition = ::std::numeric_limits<int>::max();

	// Find the barrier that has buffered data with the lowest position
	for(typename BarriersContainer::iterator it = myBarriers.begin(), end = myBarriers.end();
		it != end; ++it)
	{
		Barrier<PipelineData, SharedData, GlobalData> * const bar = *it;
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

template ABSTRACT_PIPELINE_TPARAMS
void ABSTRACT_PIPELINE_TTYPE::changeState(const PipelineState::Value newState)
{
  const PipelineState::Value oldState = myState;
  myState = newState;

  // Create the message to send out
  event::PipeStateChanged<IPipelineTyp> evt(*this, oldState, newState);
  myPipeListenerSupport.notify(evt);
}

}

#endif /* ABSTRACT_PIPELINE_H */
