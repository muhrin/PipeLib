/*
 * BlockIterator.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BLOCK_ITERATOR_H
#define BLOCK_ITERATOR_H

// INCLUDES /////////////////////////////////////////////
#include <list>
#include <set>

#include <boost/iterator/iterator_facade.hpp>

#include "pipelib/Block.h"

namespace pipelib {

template< class BlockType, class Incrementer>
  class BlockIterator : public ::boost::iterator_facade< BlockIterator< BlockType, Incrementer>,
      typename BlockType::HandleType, ::boost::forward_traversal_tag>
  {
    typedef typename BlockType::HandleType BlockHandleType;
  public:
    BlockIterator();
    BlockIterator(BlockHandleType block);

  private:
    typedef ::std::list< BlockHandleType> ToVisit;
    typedef ::std::set< BlockHandleType> Visited;

    void
    increment();

    BlockHandleType &
    dereference() const;

    bool
    equal(const BlockIterator & other) const;

    Incrementer myIncrementer;
    typename ToVisit::iterator myVisiting;
    ToVisit myToVisit;
    Visited myVisited;

    friend class boost::iterator_core_access;
  };

template< class BlockType>
  class PreorderIncrementer
  {
    typedef typename BlockType::HandleType BlockHandleType;
  public:
    typedef ::std::list< BlockHandleType> ToVisit;
    typedef ::std::set< BlockHandleType> Visited;

    template< class InputIterator>
      void
      operator()(typename ToVisit::iterator & visiting, ToVisit & toVisit, Visited & visited,
          InputIterator first, InputIterator last) const;
  };

}

#include "pipelib/detail/BlockIterator.h"

#endif /* BLOCK_ITERATOR_H */
