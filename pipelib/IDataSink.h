/*
 * IDataSink.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef I_DATA_SINK_H
#define I_DATA_SINK_H

// INCLUDES /////////////////////////////////////////////


// FORWARD DECLARATIONS ////////////////////////////////////
namespace pipelib {

template <class DataType>
class IDataSink
{
public:

	virtual ~IDataSink() {}

	virtual void in(DataType * const data) = 0;
};

}

#endif /* I_DATA_SINK_H */
