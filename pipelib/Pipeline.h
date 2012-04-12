/*
 * Pipeline.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPELINE_H
#define PIPELINE_H

// DEFINES /////////////////////////////////////////////////

#define NULL	0

#define DEBUG	1

#if !DEBUG
	#define PASSERT(x)
#else
	#include <iostream>

	#define PASSERT(x) \
	if (! (x)) \
	{ \
		std::cout << "ERROR!! Assert " << #x << " failed\n"; \
		std::cout << " on line " << __LINE__  << "\n"; \
		std::cout << " in file " << __FILE__ << "\n";  \
	}
#endif

#define CHANNEL_DEFAULT 0
#define CHANNEL_ALL		-1
#define CHANNEL_ANY		-2
#define CHANNEL_NONE	-3


// TODO: Change code to use this type!
typedef int channel_t;

namespace pipelib {

	// FORWARD DECLARES //////////////////////////////
	class EmptyGlobalData;

	// TYPEDEFS //////////////////////////////////////
	typedef EmptyGlobalData DefaultGlobalDataTyp;
	
}


#endif /* PIPELINE_H */
