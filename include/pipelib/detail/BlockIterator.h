/*
 * BlockIterator.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BLOCK_ITERATOR_DETAIL_H
#define BLOCK_ITERATOR_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <list>
#include <set>

#include <boost/iterator/filter_iterator.hpp>


namespace pipelib {
namespace detail {

/**
/* Predicate that returns true if a block has not been visited before
/* and false otherwise.
/*
/**/
template <typename BlockType>
class NotVisitedPredicate
{
public:
  typedef ::std::set<BlockType *> Visited;

  NotVisitedPredicate(const Visited & visited):
    myVisited(visited)
  {}

  bool operator ()(BlockType * const block) const
  {
    // If the block is non-null and we haven't visited it
    // then return true.  Otherwise false.
    if(block && myVisited.find(block) == myVisited.end())
      return true;

    return false;
  }
private:
  const Visited & myVisited;
};

} // namespace detail

template <class BlockType, class Incrementer>
BlockIterator<BlockType, Incrementer>::BlockIterator()
{
  myVisiting = myToVisit.end();
}

template <class BlockType, class Incrementer>
BlockIterator<BlockType, Incrementer>::BlockIterator(BlockType & block)
{
  myVisiting = myToVisit.insert(myToVisit.end(), &block);
}

template <class BlockType, class Incrementer>
void
BlockIterator<BlockType, Incrementer>::increment()
{
  myIncrementer(myVisiting, myToVisit, myVisited);
}

template <class BlockType, class Incrementer>
BlockType &
BlockIterator<BlockType, Incrementer>::dereference() const
{
  return *(*myVisiting);
}

template <class BlockType, class Incrementer>
bool
BlockIterator<BlockType, Incrementer>::equal(const BlockIterator & other) const
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


template <class BlockType>
void
PreorderIncrementer<BlockType>::operator() (
  typename ToVisit::iterator & visiting,
  ToVisit & toVisit,
  Visited & visited) const
{
  typedef detail::NotVisitedPredicate<BlockType> NotVisitedPred;
  typedef ::boost::filter_iterator<NotVisitedPred, typename BlockType::OutputIterator> FilterIter;
  NotVisitedPred filter(visited);

  // Save pointer to the one we're visiting
  BlockType * const block = *visiting;

  // Erase it from the to visit list
  typename ToVisit::iterator it = toVisit.erase(visiting);

  // Add it to the visited list
  visited.insert(block);

  // Insert all its not yet visisted outputs
  FilterIter start(filter, block->beginOutputs(), block->endOutputs());
  FilterIter end(filter, block->endOutputs(), block->endOutputs());
  toVisit.insert(it, start, end);
  if(!toVisit.empty())
    visiting = toVisit.begin();
  else
    visiting = toVisit.end();
}


}

#endif /* BLOCK_ITERATOR_DETAIL_H */
