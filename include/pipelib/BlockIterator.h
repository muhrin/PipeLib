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

namespace pipelib {

template <class BlockType, class Incrementer>
class BlockIterator :
  public ::boost::iterator_facade<
    BlockIterator<BlockType, Incrementer>,
    BlockType,
    ::boost::forward_traversal_tag
  >
{
public:
  BlockIterator();
  BlockIterator(BlockType & block);

private:
  typedef ::std::list<BlockType *> ToVisit;
  typedef ::std::set<BlockType *> Visited;

  void increment();

  BlockType & dereference() const;

  bool equal(const BlockIterator & other) const;

  Incrementer myIncrementer;
  typename ToVisit::iterator myVisiting;
  ToVisit myToVisit;
  Visited myVisited;

  friend class boost::iterator_core_access;
};

template <class BlockType>
class PreorderIncrementer
{
public:
  typedef ::std::list<BlockType *> ToVisit;
  typedef ::std::set<BlockType *> Visited;

  template <class InputIterator>
  void operator() (
    typename ToVisit::iterator & visiting,
    ToVisit & toVisit,
    Visited & visited,
    InputIterator first,
    InputIterator last
  ) const;
};

}

#include "pipelib/detail/BlockIterator.h"

#endif /* BLOCK_ITERATOR_H */
