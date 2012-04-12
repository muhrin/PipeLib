/*
 * IPipeline.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef I_PIPELINE_H
#define I_PIPELINE_H

// INCLUDES /////////////////////////////////////////////
#include "StartBlock.h"
#include "PipeBlock.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {
	template <typename DataType, class GlobalDataType>
	class Barrier;
	template <typename DataType>
	class IDataSink;
	class EmptyGlobalData;
}

namespace pipelib {

template <class DataType, class GlobalDataType = EmptyGlobalData>
class IPipeline
{
public:

	virtual ~IPipeline() {}

	/**
	/* Spawn a child pipeline that can be used as a self contained pipeline by blocks.  Users should
	/* call this if they want a block to use a pipeline as certain resources may be shared between
	/* the parent and child pipelines e.g. threads.
	/**/
	virtual IPipeline & spawnChild() = 0;

	/**
	/* Get the current start block.
	/*/
	virtual StartBlock<DataType, GlobalDataType> * getStartBlock() const = 0;

	/**
	/* Set the starting blocks that is used when the pipeline is started.
	/**/
	virtual void setStartBlock(StartBlock<DataType, GlobalDataType> & startBlock) = 0;

	/**
	/* Connect the output of one block to the input of another using an optional
	/* output channel.
	/**/
	virtual void connect(
		Block<DataType, GlobalDataType> & outputter,
		PipeBlock<DataType, GlobalDataType> & inputtee,
		const int outChannel = CHANNEL_DEFAULT) = 0;

	/**
	/* Disconnect the output of one block from the input of another on an optional
	/* output channel.
	/**/
	virtual bool disconnect(Block<DataType, GlobalDataType> & outputter, const int outChannel = CHANNEL_DEFAULT) = 0;

	///////////////////////////////////////
	// BARRIERS

	virtual Barrier<DataType, GlobalDataType> * createBarrier() = 0;

	virtual void registerBarrier(Barrier<DataType, GlobalDataType> & barrier) = 0;

	virtual bool deregisterBarrier(Barrier<DataType, GlobalDataType> & barrier) = 0;

	// END BARRIERS ///////////////////////////

	virtual void setFinishedDataSink(IDataSink<DataType> & sink) = 0;

	/**
	/* Initialise the pipeline.  A call to this method must be made before the pipeline
	/* is started using the start() method.  After this call no more changes to the
	/* pipeline are permitted and will result in an exception being thrown.
	/*
	/*/
	virtual bool initialise() = 0;
	virtual void start() = 0;
	virtual bool hasBlock(Block<DataType, GlobalDataType> & block) const = 0;
	virtual bool hasPathBetween(const Block<DataType, GlobalDataType> & from, const Block<DataType, GlobalDataType> & to) const = 0;

	/* Get the global pipeline data.  This is shared between all blocks in the pipeline.
	/**/
	virtual GlobalDataType & getSharedData() = 0;

	///////////////////////////////////////////////////////
	// Data creation, deletaion and management methods
	///////////////////////////////////////////////////////

	/**
	/* Create a new data instance.  All blocks should use this if they intend to
	/* send data down the pipe.
	/**/
	virtual DataType & newData() = 0;

	virtual void registerNewData(DataType * const data) = 0;

	/**
	/* Drop an existing data isntance.  All blocks should use this if they want to
	/* remove data from the pipeline.
	/**/
	virtual void dropData(DataType & data) = 0;

	/**
	/* If a block wants to read/write from/to data after it has outputted it then it
	/* must tell the pipeline by making a call to this method.  Otherwise there is no
	/* guarantee that the data will not be deleted in the interim.
	/**/
	virtual void flagData(const Block<DataType, GlobalDataType> & block, DataType & data) = 0;


	virtual void unflagData(const Block<DataType, GlobalDataType> & block, DataType & data) = 0;
};

}

#endif /* I_PIPELINE_H */
