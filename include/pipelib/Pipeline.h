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

#define PIPELIB_ASSERT( B ) BOOST_ASSERT( B )

#ifdef BOOST_ASSERT_MSG
#  define PIPELIB_ASSERT_MSG( B, Msg ) BOOST_ASSERT_MSG(B, Msg)
#else
#  define PIPELIB_ASSERT_MSG( B, Msg ) BOOST_ASSERT( B )
#endif



#endif /* PIPELINE_H */
