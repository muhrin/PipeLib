/*
 * PipeEngineListener.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_RUNNER_LISTENER_H
#define PIPE_RUNNER_LISTENER_H

// INCLUDES /////////////////////////////////////////////


namespace pipelib {
namespace event {

// FORWARD DECLARATIONS ////////////////////////////////////

template <class Runner>
class PipeEngineDestroyed;
template <class Runner>
class PipeEngineStateChanged;

template <class Runner>
class PipeEngineListener
{
public:
  virtual ~PipeEngineListener() {}

  virtual void notify(const PipeEngineStateChanged<Runner> & evt) {}
  virtual void notify(const PipeEngineDestroyed<Runner> & evt) {}
};

}
}

#endif /* PIPE_RUNNER_LISTENER_H */
