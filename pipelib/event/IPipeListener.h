/*
 * IPipeListener.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef I_PIPE_LISTENER_H
#define I_PIPE_LISTENER_H

// INCLUDES /////////////////////////////////////////////


namespace pipelib {
namespace event {

// FORWARD DECLARATIONS ////////////////////////////////////

template <class Pipeline>
class PipeStateChanged;

template <class Pipeline>
class IPipeListener
{
public:
  virtual void notify(const PipeStateChanged<Pipeline> & evt) = 0;

};

}
}

#endif /* I_PIPE_LISTENER_H */
