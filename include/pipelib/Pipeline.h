/*
 * Pipeline.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPELINE_H
#define PIPELINE_H

// INCLUDES ////////////////////////////////////////////////
#include <boost/assert.hpp>

#include "pipelib/Types.h"

// DEFINES /////////////////////////////////////////////////

#ifndef NULL
	#define NULL	0
#endif

#define PIPELIB_ASSERT(x) BOOST_ASSERT(x)
#define PIPELIB_ASSERT_MSG(x,msg) BOOST_ASSERT_MSG(x,msg)


#endif /* PIPELINE_H */
