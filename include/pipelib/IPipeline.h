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
#include "pipelib/StartBlock.h"
#include "pipelib/PipeBlock.h"
#include "pipelib/PipelineState.h"



namespace pipelib {

// FORWARD DECLARATIONS ////////////////////////////////////

namespace event {

  template <class Pipeline>
  class IPipeListener;
}


template <typename PipelineData, typename SharedData, class GlobalData>
class Barrier;

template <typename PipelineData>
class IDataSink;

class EmptyGlobalData;


template <
  typename PipelineData,
  typename SharedData = DefaultSharedData,
  typename GlobalData = SharedData
>
class IPipeline
{
public:

  typedef IPipeline<PipelineData, SharedData, GlobalData> IPipelineTyp;

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
	virtual StartBlock<PipelineData, SharedData, GlobalData> * getStartBlock() const = 0;

	/**
	/* Set the starting blocks that is used when the pipeline is started.
	/**/
	virtual void setStartBlock(StartBlock<PipelineData, SharedData, GlobalData> & startBlock) = 0;

	/**
	/* Connect the output of one block to the input of another using an optional
	/* output channel.
	/**/
	virtual void connect(
		Block<PipelineData, SharedData, GlobalData> & outputter,
		PipeBlock<PipelineData, SharedData, GlobalData> & inputtee,
		const ChannelTyp outChannel = CHANNEL_DEFAULT) = 0;

	/**
	/* Disconnect the output of one block from the input of another on an optional
	/* output channel.
	/**/
	virtual bool disconnect(Block<PipelineData, SharedData, GlobalData> & outputter, const ChannelTyp outChannel = CHANNEL_DEFAULT) = 0;

	///////////////////////////////////////
	// BARRIERS

	virtual void registerBarrier(Barrier<PipelineData, SharedData, GlobalData> & barrier) = 0;

	virtual bool deregisterBarrier(Barrier<PipelineData, SharedData, GlobalData> & barrier) = 0;

	// END BARRIERS ///////////////////////////

	virtual void setFinishedDataSink(IDataSink<PipelineData> & sink) = 0;

  virtual PipelineState::Value getState() const = 0;

	/**
	/* Initialise the pipeline.  A call to this method must be made before the pipeline
	/* is started using the start() method.  After this call no more changes to the
	/* pipeline are permitted and will result in an exception being thrown.
	/*
	/*/
	virtual bool initialise() = 0;
	virtual void start() = 0;
	virtual bool hasBlock(Block<PipelineData, SharedData, GlobalData> & block) const = 0;
	virtual bool hasPathBetween(const Block<PipelineData, SharedData, GlobalData> & from, const Block<PipelineData, SharedData, GlobalData> & to) const = 0;

  virtual const IPipelineTyp * getParent() const = 0;

	/**
  /* Get the shared pipeline data.  This is shared between all blocks in the pipeline.
	/**/
	virtual SharedData & getSharedData() = 0;
  virtual const SharedData & getSharedData() const = 0;

	/**
  /* Get the global pipeline data.  This is shared between all pipelines, including any children.
	/**/
	virtual GlobalData & getGlobalData() = 0;
  virtual const GlobalData & getGlobalData() const = 0;

	///////////////////////////////////////////////////////
	// Data creation, deletaion and management methods
	///////////////////////////////////////////////////////

	/**
	/* Create a new data instance.  All blocks should use this if they intend to
	/* send data down the pipe.
	/**/
	virtual PipelineData & newData() = 0;

	virtual void registerNewData(PipelineData * const data) = 0;

	/**
	/* Drop an existing data isntance.  All blocks should use this if they want to
	/* remove data from the pipeline.
	/**/
	virtual void dropData(PipelineData & data) = 0;

	/**
	/* If a block wants to read/write from/to data after it has outputted it then it
	/* must tell the pipeline by making a call to this method.  Otherwise there is no
	/* guarantee that the data will not be deleted in the interim.
	/**/
	virtual void flagData(const Block<PipelineData, SharedData, GlobalData> & block, PipelineData & data) = 0;


	virtual void unflagData(const Block<PipelineData, SharedData, GlobalData> & block, PipelineData & data) = 0;

  //////////////////////////////////////////////////////////
  // Event methods
  //////////////////////////////////////////////////////////

  virtual void addPipeListener(event::IPipeListener<IPipelineTyp> & listener) = 0;
  virtual bool removePipeListener(event::IPipeListener<IPipelineTyp> & listener) = 0;
};

}

#endif /* I_PIPELINE_H */
