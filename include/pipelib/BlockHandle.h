/*
 * BlockHandle.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BLOCK_HANDLE_H
#define BLOCK_HANDLE_H

// INCLUDES /////////////////////////////////////////////

#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>

namespace pipelib {

// FORWARD DECLARATIONS ////////////////////////////////////

template< typename Pipe, typename Shared, typename Global>
class Block;

template< typename Pipe, typename Shared, typename Global>
struct BlockHandle
{
private:
  typedef ::boost::shared_ptr< Block<Pipe, Shared, Global> > HandleType;
  BlockHandle() {}
public:
  typedef HandleType Type;
};

template<class TypedBlock>
struct TypedBlockHandle
{
private:
  typedef Block< typename TypedBlock::PipeType, typename TypedBlock::SharedType,
      typename TypedBlock::GlobalType> BlockType;
  typedef ::boost::shared_ptr<TypedBlock> HandleType;

  BOOST_STATIC_ASSERT((::boost::is_base_of<BlockType, TypedBlock>::value));

  TypedBlockHandle() {}
public:
  typedef HandleType Type;
};

}


#endif /* BLOCK_HANDLE_H */
