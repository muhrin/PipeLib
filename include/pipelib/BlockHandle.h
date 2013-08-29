/*
 * BlockHandle.h
 *
 *
 *  Created on: Aug 29, 2013
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
  typedef ::boost::shared_ptr< Block< Pipe, Shared, Global> > HandleType;
  BlockHandle() {}
public:
  typedef HandleType Type;
};

template< typename TypedBlock>
struct TypedBlockHandle
{
private:
  typedef ::boost::shared_ptr<TypedBlock> HandleType;
  typedef Block< typename TypedBlock::PipelineData, typename TypedBlock::SharedData,
      typename TypedBlock::GlobalDataType> BlockType;
  BOOST_STATIC_ASSERT((::boost::is_base_of<BlockType, TypedBlock>::value));
  TypedBlockHandle() {}
public:
  typedef HandleType Type;
};


#endif /* BLOCK_H */
