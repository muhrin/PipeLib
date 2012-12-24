/*
 * Sinks.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef SINKS_H
#define SINKS_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {

template <class PipelineData>
class FinishedSink
{
public:
  typedef typename UniquePtr<PipelineData>::Type PipelineDataPtr;

	virtual ~FinishedSink() {}

	virtual void finished(PipelineDataPtr data) = 0;
};

template <class PipelineData>
class DroppedSink
{
public:
  typedef typename UniquePtr<PipelineData>::Type PipelineDataPtr;

	virtual ~DroppedSink() {}

	virtual void dropped(PipelineDataPtr data) = 0;
};

}

#endif /* SINKS_H */
