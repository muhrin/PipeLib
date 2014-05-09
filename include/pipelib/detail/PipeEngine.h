/*
 * PipeEngine.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef PIPE_ENGINE_DETAIL_H
#define PIPE_ENGINE_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <algorithm>
#include <limits>
#include <map>

namespace pipelib {
namespace detail {

template< class BlockType>
struct SortBarriersVisitor
{
  typedef typename BlockType::HandleType BlockHandleType;
  typedef Barrier<typename BlockType::PipeType, typename BlockType::SharedType,
      typename BlockType::GlobalType> BarrierType;
  typedef ::std::map<BlockType *, ::std::pair<BarrierType *, int> > BarrierDepths;

  template< typename InputIterator>
  SortBarriersVisitor(const InputIterator first, const InputIterator last)
  {
    for(InputIterator it = first; it != last; ++it)
      barrierDepths[*it] = ::std::make_pair(*it, -1);
  }

  bool operator ()(const BlockHandleType & block, const int depth)
  {
    const typename BarrierDepths::iterator it = barrierDepths.find(block.get());

    if(it != barrierDepths.end())
      it->second.second = ::std::max(it->second.second, depth);

    return true;
  }

  BarrierDepths barrierDepths;
};

} // namespace detail


template< typename Pipe, typename Shared, typename Global>
void PipeEngine<Pipe, Shared, Global>::sortBarriers(Barriers * const barriers) const
{
  typedef detail::SortBarriersVisitor<BlockType> SortVisitor;

  SortVisitor visitor(barriers->begin(), barriers->end());
  getStartBlock()->visitBlocks(visitor);

  barriers->clear();
  while(!visitor.barrierDepths.empty())
  {
    typename SortVisitor::BarrierDepths::iterator next = visitor.barrierDepths.end();
    for(typename SortVisitor::BarrierDepths::iterator it = visitor.barrierDepths.begin(),
        end = visitor.barrierDepths.end(); it != end; ++it)
    {
      if(next == end || next->second.second > it->second.second)
        next = it;
    }
    barriers->push_back(next->second.first);
    visitor.barrierDepths.erase(next);
  }
}

}

#endif /* PIPE_ENGINE_DETAIL_H */
