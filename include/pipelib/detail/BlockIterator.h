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

namespace pipelib {

template <class BlockType, class Incrementer>
BlockIterator<BlockType, Incrementer>::BlockIterator()
{
  myVisiting = myToVisit.end();
}

template <class BlockType, class Incrementer>
BlockIterator<BlockType, Incrementer>::BlockIterator(BlockHandleType block)
{
  // Put the current block at the end of the list
  myVisiting = myToVisit.insert(myToVisit.end(), block);
}

template <class BlockType, class Incrementer>
void
BlockIterator<BlockType, Incrementer>::increment()
{
  myIncrementer(
    myVisiting,
    myToVisit,
    myVisited,
    (*myVisiting)->beginOutputs(),
    (*myVisiting)->endOutputs()
  );
}

template <class BlockType, class Incrementer>
typename BlockIterator<BlockType, Incrementer>::BlockHandleType &
BlockIterator<BlockType, Incrementer>::dereference() const
{
  return *myVisiting;
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
template <class InputIterator>
void
PreorderIncrementer<BlockType>::operator() (
  typename ToVisit::iterator & visiting,
  ToVisit & toVisit,
  Visited & visited,
  InputIterator first,
  InputIterator last
) const
{
  // Save handle to the one we're visiting
  BlockHandleType block = *visiting;

  // Erase it from the to visit list
  typename ToVisit::iterator it = toVisit.erase(visiting);

  // Add it to the visited list
  visited.insert(block);

  // Insert all its not yet visited outputs
  for(InputIterator out = first; out != last; ++out)
  {
    if(out->get() != NULL && visited.find(*out) == visited.end())
      toVisit.push_back(*out);
  }

  if(!toVisit.empty())
  {
    visiting = toVisit.begin();
    if(visited.find(*visiting) != visited.end())
      this->operator ()(visiting, toVisit, visited, (*visiting)->beginOutputs(),
          (*visiting)->endOutputs());
  }
  else
    visiting = toVisit.end();
}


}

#endif /* BLOCK_ITERATOR_DETAIL_H */
