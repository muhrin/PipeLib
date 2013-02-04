/*
 * BlockOwnerIterator.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BLOCK_OWNER_ITERATOR_H
#define BLOCK_OWNER_ITERATOR_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"

#include <list>
#include <set>

#include <boost/iterator/iterator_facade.hpp>

#include "pipelib/BlockIterator.h"

namespace pipelib {
namespace managed {


// FORWARD DECLARATIONS ////////////////////////////////////

namespace detail {

template <class BlockType, class Incrementer>
class BlockOwnerReverseIterator :
  public ::boost::iterator_facade<
    BlockOwnerReverseIterator<BlockType, Incrementer>,
    BlockType,
    ::boost::forward_traversal_tag
  >
{
public:
  BlockOwnerReverseIterator();
  BlockOwnerReverseIterator(BlockType & block);

private:
  typedef ::std::list<BlockType *> ToVisit;
  typedef ::std::set<BlockType *> Visited;

  void increment();

  BlockType & dereference() const;

  bool equal(const BlockOwnerReverseIterator & other) const;

  Incrementer myIncrementer;
  typename ToVisit::iterator myVisiting;
  ToVisit myToVisit;
  Visited myVisited;

  friend class boost::iterator_core_access;
};

template <class BlockType, class Incrementer>
BlockOwnerReverseIterator<BlockType, Incrementer>::BlockOwnerReverseIterator()
{
  myVisiting = myToVisit.end();
}

template <class BlockType, class Incrementer>
BlockOwnerReverseIterator<BlockType, Incrementer>::BlockOwnerReverseIterator(BlockType & block)
{
  // Put the current block at the end of the list
  myVisiting = myToVisit.insert(myToVisit.end(), &block);
}

template <class BlockType, class Incrementer>
void BlockOwnerReverseIterator<BlockType, Incrementer>::increment()
{
  myIncrementer(
    myVisiting,
    myToVisit,
    myVisited,
    (*myVisiting)->beginInputs(),
    (*myVisiting)->endInputs()
  );
}

template <class BlockType, class Incrementer>
BlockType &
BlockOwnerReverseIterator<BlockType, Incrementer>::dereference() const
{
  return *(*myVisiting);
}

template <class BlockType, class Incrementer>
bool
BlockOwnerReverseIterator<BlockType, Incrementer>::equal(
  const BlockOwnerReverseIterator & other) const
{
  // If we are both at the end then we are equal
  if(myVisiting == myToVisit.end() &&
    other.myVisiting == other.myToVisit.end())
    return true;
  // If only one of us is at the end then we're not equal
  else if(myVisiting == myToVisit.end())
    return false;
  else if(other.myVisiting == other.myToVisit.end())
    return false;

  // Otherwise if the block we're visiting is the same then we're equal
  return *myVisiting == *other.myVisiting;
}


} // namespace detail
} // namespace managed
} // namespace pipelib


#endif /* BLOCK_OWNER_ITERATOR_H */
